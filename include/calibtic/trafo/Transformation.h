#pragma once

#include <vector>
#include <ostream>
#include <stdexcept>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/version.hpp>

#include "calibtic/config.h"
#include "calibtic/trafo/Domain.h"

#include <pywrap/compat/macros.hpp>

namespace calibtic {
namespace trafo {

/// base class of all transformations, holds domain and reverse domain
class Transformation
{
public:

	virtual ~Transformation();

	/// Behavior when the transformation of a value outside of the domain is /
	/// requested.
	enum OutsideDomainBehavior {
		THROW, //!< throw an OutsideDomainException
		CLIP, //!< clip: use the domain boundary that is closer to the value
		IGNORE //!< ignore the domain boundaries
	};

#ifndef PYPLUSPLUS
	// sets the domain to calibtic's min/max
	Transformation();

	/// applies the transformation to @param in
	/// @param outside_domain_behavior determines the behavior if @param in is
	/// outside of the stored domain
	//
	//  the default clipping behavior will be different when called
	//  via a pointer to a class further down in the hierarchy
	virtual float_type apply(
	    float_type const& in,
	    OutsideDomainBehavior outside_domain_behavior = CLIP) const = 0;

	/// applies the transformation in reverse to @param in
	/// @param outside_domain_behavior determines the behavior if @param in is
	/// outside of the stored domain
	//
	//  the default clipping behavior will be different when called
	//  via a pointer to a class further down in the hierarchy
	virtual float_type reverseApply(
	    float_type const& in,
	    OutsideDomainBehavior outside_domain_behavior = CLIP) const = 0;

	// operator== needs to be removed for code generation, otherwise #!&($-PY++
	// will emit code, which tries to instantiate this abstract class.
	virtual bool
	operator== (Transformation const& rhs) const = 0;

	/// the domain of validity for the parameter to apply
	domain_type  getDomain() const;

	/// the domain of validity for the parameter to apply
	domain_type& getDomain();

	/// the domain of validity for the parameter to reverseApply
	domain_type  getReverseDomain() const;

	/// the domain of validity for the parameter to reverseApply
	domain_type& getReverseDomain();

	/// depending on @param outside_domain_behavior @param val is
	/// clipped to the domain boundaries, returned as is or an exception is thrown
	float_type respectDomain(float_type val,
	                         OutsideDomainBehavior outside_domain_behavior) const;

	/// depending on @param outside_domain_behavior @param val is
	/// clipped to the domain boundaries, returned as is or an exception is thrown
	float_type respectReverseDomain(float_type val,
	                                OutsideDomainBehavior outside_domain_behavior) const;

#endif // PYPLUSPLUS

	virtual std::ostream&
	operator<< (std::ostream& os) const = 0;

	/// sets the domain and also calculates and sets the reverse domain
	void setDomain(float_type min, float_type max);

	/// checks if @param val is within @param domain
	bool in_domain(float_type const& val, const domain_type& domain) const;

	PYPP_INSTANTIATE(domain_boundaries);

	/// returns the domain boundaries
	domain_boundaries getDomainBoundaries() const;

	/// returns the reverse domain boundaries
	domain_boundaries getReverseDomainBoundaries() const;

protected:

	domain_type mDomain;
	domain_type mReverseDomain;

private:
	float_type respectDomainImpl(float_type val,
	                             OutsideDomainBehavior outside_domain_behavior,
	                             const domain_type& domain) const;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const version) {

		using namespace boost::serialization;

		switch(version) {
		case 0: {
			// domain and reverse domain are set in c'tor
			break;
		}
		case 1: {
			ar & make_nvp("domain", mDomain)
			   & make_nvp("reverse_domain", mReverseDomain);
			break;
		}
		default: {
			throw std::runtime_error(
				"calibtic::Transformation unknown serialization version");
		}
		}
}

};

std::ostream& operator<< (std::ostream& os, Transformation const& t);

} // trafo
} // calibtic

BOOST_CLASS_VERSION(::calibtic::trafo::Transformation, 1)
BOOST_SERIALIZATION_ASSUME_ABSTRACT(::calibtic::trafo::Transformation)

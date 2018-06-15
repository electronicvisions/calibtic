#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/vector.hpp>
#include <limits>

#include <pywrap/compat/macros.hpp>

#include "calibtic/trafo/Transformation.h"

namespace calibtic {
namespace trafo {

class Polynomial :
	public Transformation
{
public:
	typedef std::vector<float_type> data_type;

	Polynomial(data_type const& coeff = data_type(),
			float_type const& min = 0.0,
			float_type const& max = CALIBTIC_DOMAIN_MAX);
	virtual ~Polynomial();

	virtual float_type
	apply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::CLIP) const;

	virtual float_type
	reverseApply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::CLIP) const;

	// Return real roots found by gsl_poly_complex_solve
	// @param val: solve for $f(x) = val$
	std::vector< std::complex<float_type> >
	find_roots(const float_type val) const;

	// As find_roots, but return only the real solutions
	// @param val: solve for $f(x) = val$
	// @param in_domain: filter also for the domain of the neuron
	virtual data_type
	find_real_roots(float_type const val, bool in_domain) const;

	// Returns the degree of the polynomial
	size_t degree() const;

	virtual bool
	operator== (Transformation const& rhs) const;

	virtual std::ostream&
	operator<< (std::ostream& os) const;

	data_type const& getData() const;

#ifndef PYPLUSPLUS
	data_type&       getData();
#endif // PYPLUSPLUS

	// factory function for Py++
	static
	boost::shared_ptr<Polynomial> create(
		data_type const& coeff = data_type(),
		float_type const& min = 0.0,
		float_type const& max = CALIBTIC_DOMAIN_MAX);

private:
	data_type mData;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const version)
	{
		using namespace boost::serialization;
		//ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Transformation)
		//   & make_nvp("data", mData);

		switch(version) {
		case 0: {
			// old Polynomial without domain
			data_type old;
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Transformation)
			   & make_nvp("data", old);
			*this = Polynomial(old,
					-std::numeric_limits<float_type>::infinity(),
					std::numeric_limits<float_type>::infinity());
			break;
		}
		case 1: {
			// domain was not serialized
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Transformation)
			   & make_nvp("data", mData)
			   ;//& make_nvp("domain", mDomain);
			break;
		}
		case 2: {
			// data and domain are serialized
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Transformation)
			   & make_nvp("data", mData)
			   & make_nvp("domain", mDomain);
			break;
		}
		case 3: {
			// domain now serialized in Transformation
			ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Transformation)
				& make_nvp("data", mData);
			break;
		}
		default: {
			throw std::runtime_error("calibtic::Polynomial unknown serialization version");
		}
		}
	}
};

} // trafo
} // calibtic

BOOST_CLASS_VERSION(::calibtic::trafo::Polynomial, 3)

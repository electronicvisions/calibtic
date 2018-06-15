#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/icl/continuous_interval.hpp>
#include <limits>

#include "calibtic/trafo/Polynomial.h"

namespace calibtic {
namespace trafo {

// calculate ... + a * (1/x)^2 + b * 1/x + c
class NegativePowersPolynomial :
	public Polynomial
{
public:
	typedef std::vector<float_type> data_type;

	NegativePowersPolynomial(data_type const& coeff = data_type(),
			float_type const& min = 0.0,
			float_type const& max = CALIBTIC_DOMAIN_MAX);
	virtual ~NegativePowersPolynomial();

	virtual float_type
	apply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::THROW) const;

	virtual float_type
	reverseApply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::THROW) const;

	virtual bool
	operator== (Transformation const& rhs) const;

	virtual std::ostream&
	operator<< (std::ostream& os) const;

	// factory function for Py++
	static
	boost::shared_ptr<NegativePowersPolynomial> create(
		data_type const& coeff = data_type(),
		float_type const& min = 0.0,
		float_type const& max = CALIBTIC_DOMAIN_MAX);

	virtual data_type
	find_real_roots(float_type const val, bool in_domain) const;

private:
	data_type mData;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const /* version */)
	{
		using namespace boost::serialization;
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Polynomial);
	}
};

} // trafo
} // calibtic

BOOST_CLASS_VERSION(::calibtic::trafo::NegativePowersPolynomial, 1)

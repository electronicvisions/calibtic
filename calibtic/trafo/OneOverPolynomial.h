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

/// Calculate: 1/(... + a*x^2 + b*x + c)
class OneOverPolynomial :
	public Transformation
{
public:
	typedef std::vector<float_type> data_type;

	OneOverPolynomial(data_type const& coeff = data_type(),
			float_type const& min = 0.0,
			float_type const& max = CALIBTIC_DOMAIN_MAX);
	virtual ~OneOverPolynomial();

	virtual float_type
	apply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::CLIP) const;

	virtual float_type
	reverseApply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::CLIP) const;

	virtual std::ostream&
	operator<< (std::ostream& os) const;

	virtual bool
	operator== (Transformation const& rhs) const;

	data_type const& getData() const;

#ifndef PYPLUSPLUS
	data_type&       getData();
#endif // PYPLUSPLUS

	// factory function for Py++
	static
	boost::shared_ptr<OneOverPolynomial> create(
		data_type const& coeff = data_type(),
		float_type const& min = 0.0,
		float_type const& max = CALIBTIC_DOMAIN_MAX);

private:

	Polynomial mPolynomial;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const /* version */)
	{
		using namespace boost::serialization;
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Transformation)
		   & make_nvp("polynomial", mPolynomial);
	}
};

} // trafo
} // calibtic

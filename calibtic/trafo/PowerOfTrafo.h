#pragma once

#include <vector>
#include <ostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>

#include "calibtic/config.h"

#include "calibtic/trafo/Transformation.h"

namespace calibtic {
namespace trafo {

/// Implements a transformation which computes x^y. x is another
/// transformation and y a floating point value.
class PowerOfTrafo : public Transformation
{
public:

	typedef boost::shared_ptr<Transformation> trafo_ptr;

	PowerOfTrafo(double power=0,
				 trafo_ptr t=trafo_ptr(),
				 float_type const& min = CALIBTIC_DOMAIN_MIN,
				 float_type const& max = CALIBTIC_DOMAIN_MAX);

	~PowerOfTrafo() {};

	float_type apply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::CLIP) const;

	float_type reverseApply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::CLIP) const;

	bool operator== (Transformation const& rhs) const;

	std::ostream& operator<< (std::ostream& os) const;

	// factory function for Py++
	static
	boost::shared_ptr<PowerOfTrafo> create(double power, trafo_ptr t);

private:

	double mPower;
	trafo_ptr mTrafo;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const)
	{
		using namespace boost::serialization;
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Transformation)
			& make_nvp("trafo", mTrafo)
			& make_nvp("power", mPower);
	}


};


} // trafo
} // calibtic


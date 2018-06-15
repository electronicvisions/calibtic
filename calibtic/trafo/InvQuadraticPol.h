#pragma once

#include <array>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/array.h>
#include <boost/icl/continuous_interval.hpp>
#include <limits>

#include "calibtic/trafo/Transformation.h"


namespace calibtic {
namespace trafo {

//calculate: ( mData[0] + sqrt( mData[0]² + mData[1] * (mData[2] + x) ) ) / mData[3]
class InvQuadraticPol :
	public Transformation
{
public:
	typedef std::array<float_type, 4> data_type;

	InvQuadraticPol();

	InvQuadraticPol(data_type const& coeff,
					bool negativeSolution=false,
			        float_type const& min=0.0,
			        float_type const& max=CALIBTIC_DOMAIN_MAX);

    virtual ~InvQuadraticPol();

	virtual float_type apply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::CLIP) const;

	virtual float_type
	reverseApply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::CLIP) const;

	virtual bool operator== (Transformation const& rhs) const;

	virtual std::ostream& operator<< (std::ostream& os) const;

	// factory function for Py++
	static
	boost::shared_ptr<InvQuadraticPol> create(
		data_type const& coeff = data_type(),
		bool negative = false,
		float_type const& min = 0.0,
        float_type const& max = CALIBTIC_DOMAIN_MAX);

private:
	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const)
	{
		using namespace boost::serialization;
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Transformation)
		   & make_nvp("data", mData)
		   & make_nvp("signum", mSignum);
	}

	data_type mData;
	int mSignum;

};

} //end namespace trafo
} //end namespace calibtic

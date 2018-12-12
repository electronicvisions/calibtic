#include "calibtic/trafo/InvQuadraticPol.h"

#include <cmath>
#include <iostream>

namespace calibtic {
namespace trafo {

InvQuadraticPol::InvQuadraticPol() : mData({{0, 0, 0, 1}}), mSignum(1) {
	setDomain(0.0, CALIBTIC_DOMAIN_MAX);
}

InvQuadraticPol::InvQuadraticPol(   data_type const& coeff,
                                    bool negativeSolution,
			                        float_type const& min,
			                        float_type const& max) :
	mData(coeff),
	mSignum(negativeSolution ? -1 : 1) {
	setDomain(min,max);
}

InvQuadraticPol::~InvQuadraticPol() {}

float_type InvQuadraticPol::apply(float_type const& in, OutsideDomainBehavior outside_domain_behavior) const
{
    if (mData.size() < 4) {
		throw std::runtime_error("invalid data set");
	}

	const float_type val = respectDomain(in, outside_domain_behavior);

	double const r = pow(mData[0], 2) + mData[1]*(mData[2] + val);

	if(r < 0) {
		throw std::runtime_error("imaginary result");
	}

	return (mData[0] + mSignum*sqrt(r)) / mData[3];
}

float_type
InvQuadraticPol::reverseApply(float_type const& /*in*/, Transformation::OutsideDomainBehavior /*outside_domain_behavior*/) const
{
	throw std::runtime_error("Not implemented");
}

bool InvQuadraticPol::operator== (Transformation const& rhs) const
{
	InvQuadraticPol const* _rhs = dynamic_cast<InvQuadraticPol const*>(&rhs);
	if (!_rhs) {
		return false;
	}
	return (mData == _rhs->mData) && (mDomain == _rhs->mDomain);
}

std::ostream& InvQuadraticPol::operator<< (std::ostream& os) const
{
	os << "InvQuadraticPol: ";
	for (auto const& val : mData)
	{
		os << val << ", ";
	}
	os << ", domain: " << mDomain;
	return os;
}

boost::shared_ptr<InvQuadraticPol>
InvQuadraticPol::create(data_type const& coeff, 
                        bool negative,
		                float_type const& min,
                        float_type const& max)
{
	return boost::shared_ptr<InvQuadraticPol>(new InvQuadraticPol(coeff, negative, min, max));
}

} //end namespace trafo
} //end namespace calibtic

#include "calibtic/trafo/PowerOfTrafo.h"
#include <iostream>

namespace calibtic {
namespace trafo {

PowerOfTrafo::PowerOfTrafo(double power, trafo_ptr t, float_type const& min,
                           float_type const& max)
    : mPower(power), mTrafo(t) {
	setDomain(min, max);
}

float_type PowerOfTrafo::reverseApply(float_type const& /*in*/, Transformation::OutsideDomainBehavior /*outside_domain_behavior*/) const {
	throw std::runtime_error("PowerOfTrafo cannot be reversed.");
}

bool PowerOfTrafo::operator==(Transformation const& rhs) const {

	// compare type
	PowerOfTrafo const* _rhs = dynamic_cast<PowerOfTrafo const*>(&rhs);
	if (!_rhs) {
		return false;
	}

	// compare trafo

	if (mPower == _rhs->mPower) {
		if (mTrafo && _rhs->mTrafo) { // both trafos exist
			return *mTrafo == *_rhs->mTrafo;
		} else if (mTrafo == _rhs->mTrafo) { // both null
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

boost::shared_ptr<PowerOfTrafo> PowerOfTrafo::create(double power, trafo_ptr t) {
		return boost::shared_ptr<PowerOfTrafo>(new PowerOfTrafo(power, t));
}

float_type PowerOfTrafo::apply(float_type const& in, OutsideDomainBehavior outside_domain_behavior) const {

	float_type val = respectDomain(in, outside_domain_behavior);

	float_type res = 0;

	if(mTrafo) {
		res = pow(mTrafo->apply(val), mPower);
	} else {
		res = pow(val, mPower);
	}

	return res;
}

std::ostream& PowerOfTrafo::operator<<(std::ostream& os) const {
	os << "PowerOfTrafo: " << "(" << *mTrafo << ")^" << mPower << '\n';

	return os;
}


} // trafo
} // calibtic

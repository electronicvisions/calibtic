#include "calibtic/trafo/SumOfTrafos.h"
#include <iostream>

namespace calibtic {
namespace trafo {

SumOfTrafos::SumOfTrafos(const trafo_list& trafos, float_type const& min,
                         float_type const& max)
    : mTrafos(trafos) {
	setDomain(min, max);
}

float_type SumOfTrafos::reverseApply(float_type const& /*in*/, Transformation::OutsideDomainBehavior /*outside_domain_behavior*/) const {
	throw std::runtime_error("SumOfTrafos cannot be reversed.");
}

bool SumOfTrafos::operator==(Transformation const& rhs) const {

	// compare type
	SumOfTrafos const* _rhs = dynamic_cast<SumOfTrafos const*>(&rhs);
	if (!_rhs) {
		return false;
	}

	// compare number of trafos
	if (mTrafos.size() != _rhs->mTrafos.size()) {
		return false;
	}

	// compare trafos
	// FIXME: order of trafos shouldn't matter
	for (size_t i = 0; i != mTrafos.size(); ++i) {
		if (!(*mTrafos[i] == *_rhs->mTrafos[i])) {
			return false;
		}
	}

	return true;
}

boost::shared_ptr<SumOfTrafos> SumOfTrafos::create(trafo_list const& trafos) {
	return boost::shared_ptr<SumOfTrafos>(new SumOfTrafos(trafos));
}

float_type SumOfTrafos::apply(float_type const& in, OutsideDomainBehavior outside_domain_behavior) const {

	float_type val = respectDomain(in, outside_domain_behavior);

	float_type res = 0;

	for (auto t : mTrafos) {
		res += t->apply(val);
	}

	return res;
}

std::ostream& SumOfTrafos::operator<<(std::ostream& os) const {
	os << "SumOfTrafos: " << '\n';
	for (auto t : mTrafos) {
		os << '\t' << *t << '\n';
	}
	return os;
}


} // trafo
} // calibtic

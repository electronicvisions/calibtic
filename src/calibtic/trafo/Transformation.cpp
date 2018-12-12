#include "calibtic/trafo/Transformation.h"

#include <log4cxx/logger.h>
#include <cmath>

namespace calibtic {
namespace trafo {

static log4cxx::LoggerPtr _log = log4cxx::Logger::getLogger("Calibtic");

Transformation::Transformation() {
	mDomain = boost::icl::construct<boost::icl::continuous_interval<float_type> >(
	    CALIBTIC_DOMAIN_MIN, CALIBTIC_DOMAIN_MAX, boost::icl::interval_bounds::closed());
	mReverseDomain = boost::icl::construct<boost::icl::continuous_interval<float_type> >(
	    CALIBTIC_DOMAIN_MIN, CALIBTIC_DOMAIN_MAX, boost::icl::interval_bounds::closed());
}

Transformation::~Transformation() {}

bool Transformation::in_domain(float_type const& val, const domain_type& domain) const {
	return boost::icl::contains(domain, val);
}

domain_boundaries Transformation::getDomainBoundaries() const {
	return {mDomain.lower(), mDomain.upper()};
}

domain_boundaries Transformation::getReverseDomainBoundaries() const {
	return {mReverseDomain.lower(), mReverseDomain.upper()};
}

calibtic::domain_type Transformation::getDomain() const { return mDomain; }

calibtic::domain_type& Transformation::getDomain() { return mDomain; }

calibtic::domain_type Transformation::getReverseDomain() const { return mReverseDomain; }

calibtic::domain_type& Transformation::getReverseDomain() { return mReverseDomain; }

void Transformation::setDomain(float_type min, float_type max) {
	mDomain = boost::icl::construct<boost::icl::continuous_interval<float_type> >(
	    min, max, boost::icl::interval_bounds::closed());

	float_type const a_min = apply(min);
	float_type const a_max = apply(max);

	float_type const reverse_min = a_min < a_max ? a_min : a_max;
	float_type const reverse_max = a_max > a_min ? a_max : a_min;

	float_type const strip_inf_reverse_min =
	    std::isinf(reverse_min) ? CALIBTIC_DOMAIN_MIN : reverse_min;
	float_type const strip_inf_reverse_max =
	    std::isinf(reverse_max) ? CALIBTIC_DOMAIN_MAX : reverse_max;

	if(std::isnan(reverse_min) || std::isnan(reverse_max)) {
		throw std::runtime_error("Transformation::setDomain calculcated NaN for reverse domain");
	}

	mReverseDomain = boost::icl::construct<boost::icl::continuous_interval<float_type> >(
	    strip_inf_reverse_min, strip_inf_reverse_max,
	    boost::icl::interval_bounds::closed());
}

float_type Transformation::respectDomain(
    float_type in, OutsideDomainBehavior outside_domain_behavior) const {
	return respectDomainImpl(in, outside_domain_behavior, mDomain);
}

float_type Transformation::respectReverseDomain(
    float_type in, OutsideDomainBehavior outside_domain_behavior) const {
	return respectDomainImpl(in, outside_domain_behavior, mReverseDomain);
}

float_type Transformation::respectDomainImpl(
    float_type in, OutsideDomainBehavior outside_domain_behavior,
    const domain_type& domain) const {

	float_type val = in;

	if (!in_domain(in, domain)) {
		switch (outside_domain_behavior) {
			case OutsideDomainBehavior::THROW: {
				std::stringstream err_msg;
				err_msg << "Value " << in << " outside of domain " << domain;
				throw OutsideDomainException(err_msg.str());
				break;
			}
			case OutsideDomainBehavior::CLIP: {

				val = (in <= domain.lower()) ? domain.lower() : domain.upper();

				LOG4CXX_WARN(_log, "Transformation::respectDomain: input value "
				                       << in << " outside of domain " << domain
				                       << ", clipped to " << val << ".");

				break;
			}
			case OutsideDomainBehavior::IGNORE: {

				break;
			}
		}
	}

	return val;
}

std::ostream& operator<<(std::ostream& os, calibtic::trafo::Transformation const& t) {
	return t.operator<<(os);
}

} // trafo
} // calibtic

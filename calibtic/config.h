#pragma once

#include <utility>
#include <boost/icl/continuous_interval.hpp>
#include <boost/serialization/continuous_interval.h>

namespace calibtic {

typedef double float_type;

typedef boost::icl::continuous_interval<float_type> domain_type;

typedef std::pair<float_type, float_type> domain_boundaries;

// infinity not supported for text based (e.g. xml) archives, see:
// http://stackoverflow.com/a/28701818/1350789
const float_type CALIBTIC_DOMAIN_MIN = -std::numeric_limits<float_type>::max();
const float_type CALIBTIC_DOMAIN_MAX = std::numeric_limits<float_type>::max();

}

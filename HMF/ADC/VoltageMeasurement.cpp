#include "HMF/ADC/VoltageMeasurement.h"

#include <algorithm>

namespace HMF {
namespace ADC {

void VoltageMeasurement::push_back(DataPoint const& point)
{
	mData.push_back(point);
}

void VoltageMeasurement::check() const
{
	data_type::const_iterator it =
		std::max_element(mData.begin(), mData.end(),
						 [](DataPoint const& a, DataPoint const& b) {
							return a.std < b.std;
						 });

	static_cast<void>(it);
}

size_t VoltageMeasurement::size() const
{
	return mData.size();
}

VoltageMeasurement::data_type const&
VoltageMeasurement::data() const
{
	return mData;
}

} // ADC
} // HMF

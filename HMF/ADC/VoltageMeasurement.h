#pragma once
#include <vector>
#include <cstdlib>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

#include "HMF/ADC/DataPoint.h"

namespace HMF {
namespace ADC {

class VoltageMeasurement
{
private:
	typedef std::vector<DataPoint> data_type;

public:
	void push_back(DataPoint const& point);
	void check() const;

	size_t size() const;

#ifndef PYPLUSPLUS
	data_type const& data() const;
#endif

private:
	data_type mData;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};

} // ADC
} // HMF



// implementation

namespace HMF {
namespace ADC {

template<typename Archiver>
void VoltageMeasurement::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & make_nvp("data", mData);
}

} // ADC
} // HMF

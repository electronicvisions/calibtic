#pragma once

namespace HMF {
namespace ADC {

class DataPoint
{
public:
	typedef float value_type;

	DataPoint(value_type r, value_type m, value_type s) :
		ref(r), mean(m), std(s) {}

	/// reference or ideal voltage
	value_type ref;

	/// measured mean
	value_type mean;

	/// standard deviation of measurment
	value_type std;

private:
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
void DataPoint::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & make_nvp("mean", mean)
	   & make_nvp("std", std)
	   & make_nvp("ref", ref);
}

} // ADC
} // HMF

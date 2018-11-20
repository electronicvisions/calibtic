#pragma once

#include <iostream>
#include "calibtic/Calibration.h"

namespace HMF {

class SynapseSwitchCalibration : public calibtic::Calibration
{
public:
	SynapseSwitchCalibration();
	virtual ~SynapseSwitchCalibration();

	void setDefaults();

	size_t getMaxSwitches() const;
	void setMaxSwitches(size_t const s);

	virtual std::ostream& operator<<(std::ostream&) const;

	// Py++ factory function
	static boost::shared_ptr<SynapseSwitchCalibration> create();

	virtual void copy(Calibration const& rhs);

private:
	friend class boost::serialization::access;
	template <typename Archiver>
	void serialize(Archiver& ar, unsigned int const);

	size_t mMaxSwitches;
};

} // HMF


// implementations

namespace HMF {

template <typename Archiver>
void SynapseSwitchCalibration::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Calibration) & make_nvp("MaxSwitches", mMaxSwitches);
}

} // HMF

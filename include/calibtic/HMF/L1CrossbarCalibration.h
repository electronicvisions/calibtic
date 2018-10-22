#pragma once

#include "calibtic/Calibration.h"
#include <iostream>

namespace HMF {

class L1CrossbarCalibration : public calibtic::Calibration
{
public:
	L1CrossbarCalibration();
	virtual ~L1CrossbarCalibration();

	void setDefaults();

	size_t getMaxSwitchesPerRow() const;
	void setMaxSwitchesPerRow(size_t const s);

	size_t getMaxSwitchesPerColumn() const;
	void setMaxSwitchesPerColumn(size_t const s);

	virtual std::ostream& operator<<(std::ostream&) const;

	// Py++ factory function
	static boost::shared_ptr<L1CrossbarCalibration> create();

	virtual void copy(Calibration const& rhs);

private:
	friend class boost::serialization::access;
	template <typename Archiver>
	void serialize(Archiver& ar, unsigned int const);

	size_t mMaxSwitchesPerRow;
	size_t mMaxSwitchesPerColumn;
};

} // HMF


// implementations

namespace HMF {

template <typename Archiver>
void L1CrossbarCalibration::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Calibration) &
	    make_nvp("MaxSwitchesPerRow", mMaxSwitchesPerRow) &
	    make_nvp("MaxSwitchesPerColumn", mMaxSwitchesPerColumn);
}

} // HMF

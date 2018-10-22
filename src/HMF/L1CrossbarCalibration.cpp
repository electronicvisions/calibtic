#include "calibtic/HMF/L1CrossbarCalibration.h"
#include <vector>

namespace HMF {

L1CrossbarCalibration::L1CrossbarCalibration()
{
	setDefaults();
}

L1CrossbarCalibration::~L1CrossbarCalibration() {}

void L1CrossbarCalibration::setDefaults()
{
	mMaxSwitchesPerRow = 1;
	mMaxSwitchesPerColumn = 1;
}

size_t L1CrossbarCalibration::getMaxSwitchesPerRow() const
{
	return mMaxSwitchesPerRow;
}

void L1CrossbarCalibration::setMaxSwitchesPerRow(size_t const s)
{
	mMaxSwitchesPerRow = s;
}

size_t L1CrossbarCalibration::getMaxSwitchesPerColumn() const
{
	return mMaxSwitchesPerColumn;
}

void L1CrossbarCalibration::setMaxSwitchesPerColumn(size_t const s)
{
	mMaxSwitchesPerColumn = s;
}


boost::shared_ptr<L1CrossbarCalibration> L1CrossbarCalibration::create()
{
	return boost::shared_ptr<L1CrossbarCalibration>(new L1CrossbarCalibration());
}

void L1CrossbarCalibration::copy(Calibration const& rhs)
{
	*this = dynamic_cast<L1CrossbarCalibration const&>(rhs);
}

std::ostream& L1CrossbarCalibration::operator<<(std::ostream& os) const
{
	os << "L1CrossbarCalibration:"
		<< " MaxSwitchesPerRow:    " << mMaxSwitchesPerRow
		<< " MaxSwitchesPerColumn: " << mMaxSwitchesPerColumn
		<< "\n";
	return Calibration::operator<<(os);
}

} // HMF

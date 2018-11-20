#include "calibtic/HMF/SynapseSwitchCalibration.h"
#include <vector>

namespace HMF {

SynapseSwitchCalibration::SynapseSwitchCalibration()
{
	setDefaults();
}

SynapseSwitchCalibration::~SynapseSwitchCalibration() {}

void SynapseSwitchCalibration::setDefaults()
{
	mMaxSwitches = 1;
}

size_t SynapseSwitchCalibration::getMaxSwitches() const
{
	return mMaxSwitches;
}

void SynapseSwitchCalibration::setMaxSwitches(size_t const s)
{
	mMaxSwitches = s;
}

boost::shared_ptr<SynapseSwitchCalibration> SynapseSwitchCalibration::create()
{
	return boost::shared_ptr<SynapseSwitchCalibration>(new SynapseSwitchCalibration());
}

void SynapseSwitchCalibration::copy(Calibration const& rhs)
{
	*this = dynamic_cast<SynapseSwitchCalibration const&>(rhs);
}

std::ostream& SynapseSwitchCalibration::operator<<(std::ostream& os) const
{
	os << "SynapseSwitchCalibration:"
	   << " MaxSwitches:    " << mMaxSwitches << "\n";
	return Calibration::operator<<(os);
}

} // HMF

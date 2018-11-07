#include "calibtic/HMF/SynapseChainLengthCalibration.h"
#include <vector>

namespace HMF {

SynapseChainLengthCalibration::SynapseChainLengthCalibration()
{
	setDefaults();
}

SynapseChainLengthCalibration::~SynapseChainLengthCalibration() {}

void SynapseChainLengthCalibration::setDefaults()
{
	mMaxChainLength = 3;
}

size_t SynapseChainLengthCalibration::getMaxChainLength() const
{
	return mMaxChainLength;
}

void SynapseChainLengthCalibration::setMaxChainLength(size_t const s)
{
	mMaxChainLength = s;
}

boost::shared_ptr<SynapseChainLengthCalibration> SynapseChainLengthCalibration::create()
{
	return boost::shared_ptr<SynapseChainLengthCalibration>(new SynapseChainLengthCalibration());
}

void SynapseChainLengthCalibration::copy(Calibration const& rhs)
{
	*this = dynamic_cast<SynapseChainLengthCalibration const&>(rhs);
}

std::ostream& SynapseChainLengthCalibration::operator<<(std::ostream& os) const
{
	os << "SynapseChainLengthCalibration:"
		<< " MaxChainLength:    " << mMaxChainLength
		<< "\n";
	return Calibration::operator<<(os);
}

} // HMF

#include "calibtic/HMF/SynapseChainLengthCollection.h"

#include <vector>
#include "halco/common/iter_all.h"
#include "halco/hicann/v2/l1.h"
#include "calibtic/HMF/SynapseChainLengthCalibration.h"

namespace HMF {

SynapseChainLengthCollection::SynapseChainLengthCollection() {}

SynapseChainLengthCollection::~SynapseChainLengthCollection() {}

void SynapseChainLengthCollection::setDefaults()
{
	for (size_t ii : halco::common::iter_all<halco::hicann::v2::VLineOnHICANN>()) {
		halco::hicann::v2::VLineOnHICANN line(ii);
		boost::shared_ptr<SynapseChainLengthCalibration> sclc(new SynapseChainLengthCalibration);
		sclc->setDefaults();
		erase(line.value());
		insert(line.value(), sclc);
	}
}

size_t SynapseChainLengthCollection::getMaxChainLength(halco::hicann::v2::VLineOnHICANN vline) const
{
	if (!exists(vline.value()) || !at(vline.value())) {
		throw std::runtime_error("No Calibration data for SynapseChainLengthCollection");
	}
	SynapseChainLengthCalibration const& sclc =
	    *boost::dynamic_pointer_cast<SynapseChainLengthCalibration const>(at(vline.value()));
	return sclc.getMaxChainLength();
}

void SynapseChainLengthCollection::setMaxChainLength(
    halco::hicann::v2::VLineOnHICANN vline, size_t const s)
{
	if (exists(vline.value()) && at(vline.value())) {
		SynapseChainLengthCalibration& sclc =
		    *boost::dynamic_pointer_cast<SynapseChainLengthCalibration>(at(vline.value()));
		sclc.setMaxChainLength(s);
	} else {
		boost::shared_ptr<SynapseChainLengthCalibration> sclc(new SynapseChainLengthCalibration);
		sclc->setDefaults();
		sclc->setMaxChainLength(s);
		insert(vline.value(), sclc);
	}
}

boost::shared_ptr<SynapseChainLengthCollection> SynapseChainLengthCollection::create()
{
	return boost::shared_ptr<SynapseChainLengthCollection>(new SynapseChainLengthCollection());
}

void SynapseChainLengthCollection::copy(Collection const& rhs)
{
	*this = dynamic_cast<SynapseChainLengthCollection const&>(rhs);
}

} // HMF

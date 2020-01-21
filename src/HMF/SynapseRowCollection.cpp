#include "calibtic/HMF/SynapseRowCollection.h"
#include "calibtic/HMF/SynapseRowCalibration.h"
#include "halco/hicann/v2/synapse.h"

namespace HMF {

SynapseRowCollection::SynapseRowCollection()
{
}

SynapseRowCollection::~SynapseRowCollection()
{
}

void SynapseRowCollection::setDefaults()
{
	boost::shared_ptr<SynapseRowCalibration> src(new SynapseRowCalibration);
	src->setDefaults();
	for (size_t ii = halco::hicann::v2::SynapseRowOnHICANN::min; ii<=halco::hicann::v2::SynapseRowOnHICANN::max; ++ii) {
		erase(ii);
		insert(ii, src);
	}
}

void SynapseRowCollection::setEssDefaults()
{
	boost::shared_ptr<SynapseRowCalibration> src(new SynapseRowCalibration);
	src->setEssDefaults();
	for (size_t ii = halco::hicann::v2::SynapseRowOnHICANN::min; ii<=halco::hicann::v2::SynapseRowOnHICANN::max; ++ii) {
		erase(ii);
		insert(ii, src);
	}
}

boost::shared_ptr<SynapseRowCollection> SynapseRowCollection::create()
{
	return boost::shared_ptr<SynapseRowCollection>(
		new SynapseRowCollection());
}

std::ostream& SynapseRowCollection::operator<< (std::ostream& os) const
{
	os << "SynapseRowCollection: ";
	return Collection::operator<<(os);
}

void SynapseRowCollection::copy(calibtic::Collection const& rhs)
{
	*this = dynamic_cast<SynapseRowCollection const&>(rhs);
}

} // HMF

#include "HMF/SynapseRowCollection.h"
#include "HMF/SynapseRowCalibration.h"
#include "hal/Coordinate/HMFGeometry.h"

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
	for (size_t ii = HMF::Coordinate::SynapseRowOnHICANN::min; ii<=HMF::Coordinate::SynapseRowOnHICANN::max; ++ii) {
		erase(ii);
		insert(ii, src);
	}
}

void SynapseRowCollection::setEssDefaults()
{
	boost::shared_ptr<SynapseRowCalibration> src(new SynapseRowCalibration);
	src->setEssDefaults();
	for (size_t ii = HMF::Coordinate::SynapseRowOnHICANN::min; ii<=HMF::Coordinate::SynapseRowOnHICANN::max; ++ii) {
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

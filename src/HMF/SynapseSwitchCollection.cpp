#include "calibtic/HMF/SynapseSwitchCollection.h"

#include "calibtic/HMF/SynapseSwitchCalibration.h"
#include <vector>
#include "hal/Coordinate/iter_all.h"

namespace HMF {

SynapseSwitchCollection::SynapseSwitchCollection() {}

SynapseSwitchCollection::~SynapseSwitchCollection() {}

void SynapseSwitchCollection::setDefaults()
{
	for (size_t ii : HMF::Coordinate::iter_all<HMF::Coordinate::VLineOnHICANN>()) {
		HMF::Coordinate::VLineOnHICANN line(ii);
		boost::shared_ptr<SynapseSwitchCalibration> ssc(new SynapseSwitchCalibration);
		ssc->setDefaults();
		erase(line.value());
		insert(line.value(), ssc);
	}
}

size_t SynapseSwitchCollection::getMaxSwitches(HMF::Coordinate::VLineOnHICANN vline) const
{
	if (!exists(vline.value()) || !at(vline.value())) {
		throw std::runtime_error("No Calibration data for SynapseSwitchCollection");
	}
	SynapseSwitchCalibration const& ssc =
	    *boost::dynamic_pointer_cast<SynapseSwitchCalibration const>(at(vline.value()));
	return ssc.getMaxSwitches();
}

void SynapseSwitchCollection::setMaxSwitches(HMF::Coordinate::VLineOnHICANN vline, size_t const s)
{
	if (exists(vline.value()) && at(vline.value())) {
		SynapseSwitchCalibration& ssc =
		    *boost::dynamic_pointer_cast<SynapseSwitchCalibration>(at(vline.value()));
		ssc.setMaxSwitches(s);
	} else {
		boost::shared_ptr<SynapseSwitchCalibration> ssc(new SynapseSwitchCalibration);
		ssc->setDefaults();
		ssc->setMaxSwitches(s);
		insert(vline.value(), ssc);
	}
}

boost::shared_ptr<SynapseSwitchCollection> SynapseSwitchCollection::create()
{
	return boost::shared_ptr<SynapseSwitchCollection>(new SynapseSwitchCollection());
}

void SynapseSwitchCollection::copy(Collection const& rhs)
{
	*this = dynamic_cast<SynapseSwitchCollection const&>(rhs);
}

} // HMF

#include "calibtic/HMF/BlockCollection.h"
#include "calibtic/HMF/SharedCalibration.h"
#include "hal/Coordinate/HMFGeometry.h"
#include <cmath>
#include <vector>

namespace HMF {

BlockCollection::BlockCollection()
{
}

BlockCollection::~BlockCollection()
{
}

void BlockCollection::setDefaults()
{
	boost::shared_ptr<SharedCalibration> sc(new SharedCalibration);
	sc->setDefaults();
	for (size_t ii = 0; ii<HMF::Coordinate::FGBlockOnHICANN::enum_type::size; ++ii) {
		erase(ii);
		insert(ii, sc);
	}
}

boost::shared_ptr<BlockCollection> BlockCollection::create()
{
	return boost::shared_ptr<BlockCollection>(
		new BlockCollection());
}

std::ostream& BlockCollection::operator<< (std::ostream& os) const
{
	os << "BlockCollection: ";
	return Collection::operator<<(os);
}

void BlockCollection::copy(calibtic::Collection const& rhs)
{
	*this = dynamic_cast<BlockCollection const&>(rhs);
}

HWSharedParameter BlockCollection::applySharedCalibration(double v_reset, size_t hw_shared_id) const {

	if (!exists(hw_shared_id) || !at(hw_shared_id)) {
		throw std::runtime_error("no calibration data for this fg block");
	}

	SharedCalibration const& calib =
		*boost::dynamic_pointer_cast<SharedCalibration const>(at(hw_shared_id));

	return calib.applySharedCalibration(v_reset);

}

} // HMF

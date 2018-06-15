#include "HMF/SynapseCalibration.h"

#include "calibtic/trafo/Polynomial.h"
#include "calibtic/trafo/Constant.h"

#include <type_traits>

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr _log = log4cxx::Logger::getLogger("Calibtic");

using namespace calibtic::trafo;
using namespace std;

namespace HMF {


SynapseCalibration::SynapseCalibration() :
	Calibration(1)
{
}

SynapseCalibration::~SynapseCalibration()
{}

void SynapseCalibration::setDefaults()
{
	// default synapse trafo from RTL simulation
	reset(0, Polynomial::create({93.9406513768,81.3443474483,-1.59734048208}, HICANN::SynapseWeight::min, HICANN::SynapseWeight::max));
	assert(check_monotonic_increasing());
}

HICANN::SynapseWeight SynapseCalibration::getDigitalWeight(
	double const analog_weight
	) const
{
	// TODO: several optimizations are possible:
	// 1.) pre-calculate all 16 analog weights
	// 2.) using ReverseTrafo to find digital weight
	int id_of_first_smaller = -1;
	// this assumes a monoton increasing of synaptic weights
	for (size_t dw = HICANN::SynapseWeight::min; dw <= HICANN::SynapseWeight::max; ++dw)
	{
		double aw = getAnalogWeight(HICANN::SynapseWeight(dw));
		if ( aw < analog_weight)
			id_of_first_smaller = dw;
		else
			break;
	}
	if (id_of_first_smaller == -1) {
		return HICANN::SynapseWeight(HICANN::SynapseWeight::min);
	}
	else if (id_of_first_smaller  == HICANN::SynapseWeight::max) {
		return HICANN::SynapseWeight(HICANN::SynapseWeight::max);
	}
	else {
		double aw_l = getAnalogWeight(HICANN::SynapseWeight(id_of_first_smaller));
		double aw_h = getAnalogWeight(HICANN::SynapseWeight(id_of_first_smaller + 1));
		double remainder =  (analog_weight - aw_l)/(aw_h-aw_l);
		return HICANN::SynapseWeight(id_of_first_smaller + ((double)rand() / RAND_MAX < remainder));
	}
}

SynapseCalibration::analog_weight_t
SynapseCalibration::getAnalogWeight(HICANN::SynapseWeight const digital_weight) const {
	return mTrafo[0]->apply(digital_weight);
}

SynapseCalibration::analog_weight_t SynapseCalibration::getMinAnalogWeight() const {
	return getAnalogWeight(HICANN::SynapseWeight(HICANN::SynapseWeight::min));
}

SynapseCalibration::analog_weight_t SynapseCalibration::getMaxAnalogWeight() const {
	return getAnalogWeight(HICANN::SynapseWeight(HICANN::SynapseWeight::max));
}

ostream& SynapseCalibration::operator<< (ostream& os) const
{
	os << "SynapseCalibration: ";
	return Calibration::operator<<(os);
}

boost::shared_ptr<SynapseCalibration> SynapseCalibration::create()
{
	return boost::shared_ptr<SynapseCalibration>(new SynapseCalibration());
}

void SynapseCalibration::copy(Calibration const& rhs)
{
	*this = dynamic_cast<SynapseCalibration const&>(rhs);
}

bool SynapseCalibration::check_monotonic_increasing() const
{
	for (size_t dw = HICANN::SynapseWeight::min; dw < HICANN::SynapseWeight::max; ++dw) {
		if (getAnalogWeight(HICANN::SynapseWeight(dw)) >
		    getAnalogWeight(HICANN::SynapseWeight(dw + 1))) {
			return false;
		}
	}
	return true;
}

} // HMF

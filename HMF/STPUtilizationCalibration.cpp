#include "HMF/STPUtilizationCalibration.h"

#include "calibtic/trafo/Polynomial.h"
#include "calibtic/trafo/Constant.h"

#include <type_traits>

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr _log = log4cxx::Logger::getLogger("Calibtic");

using namespace calibtic::trafo;
using namespace std;

namespace HMF {

const uint8_t STPUtilizationCalibration::minCap;
const uint8_t STPUtilizationCalibration::maxCap;

STPUtilizationCalibration::STPUtilizationCalibration() :
	Calibration(1)
{
}

STPUtilizationCalibration::~STPUtilizationCalibration()
{}

void STPUtilizationCalibration::setDefaults()
{
	// S. Billaudelle measured a range of 0.25 - 0.4
	// We assume that the utilization scales linearly between these depending on cap
	reset(0, Polynomial::create({0.25, (0.4-0.25)/7}, 0,7));
	assert(check_monotonic_increasing());
}

uint8_t STPUtilizationCalibration::getCap(
	double const utilization
	) const
{
	// TODO: this duplicates the code from SynapseCalibration
	int id_of_first_smaller = -1;
	// this assumes a monoton increasing of U_SE for increasing cap values
	for (size_t cap = minCap; cap <= maxCap; ++cap)
	{
		double u = getUtilization(cap);
		if ( u < utilization)
			id_of_first_smaller = cap;
		else
			break;
	}
	if (id_of_first_smaller == -1) {
		return minCap;
	}
	else if (id_of_first_smaller  == maxCap) {
		return maxCap;
	}
	else {
		double u_l = getUtilization(id_of_first_smaller);
		double u_h = getUtilization(id_of_first_smaller+1);
		double remainder =  (utilization - u_l)/(u_h-u_l);
		// return id_of_first_smaller +((double)rand()/RAND_MAX < remainder );
		return id_of_first_smaller + round(remainder);
	}
}

STPUtilizationCalibration::utilization_t
STPUtilizationCalibration::getUtilization(uint8_t const cap) const {
	return mTrafo[0]->apply(cap);
}

STPUtilizationCalibration::utilization_t STPUtilizationCalibration::getMinUtilization() const {
	return getUtilization(minCap);
}

STPUtilizationCalibration::utilization_t STPUtilizationCalibration::getMaxUtilization() const {
	return getUtilization(maxCap);
}

ostream& STPUtilizationCalibration::operator<< (ostream& os) const
{
	os << "STPUtilizationCalibration: ";
	return Calibration::operator<<(os);
}

boost::shared_ptr<STPUtilizationCalibration> STPUtilizationCalibration::create()
{
	return boost::shared_ptr<STPUtilizationCalibration>(new STPUtilizationCalibration());
}

void STPUtilizationCalibration::copy(Calibration const& rhs)
{
	*this = dynamic_cast<STPUtilizationCalibration const&>(rhs);
}

bool STPUtilizationCalibration::check_monotonic_increasing() const
{
	for (size_t cap = minCap; cap < maxCap; ++cap)
	{
		if (getUtilization(cap) > getUtilization(cap+1))
			return false;
	}
	return true;
}

} // HMF

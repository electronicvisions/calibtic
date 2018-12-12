#pragma once

#include "calibtic/Calibration.h"
#include "calibtic/HMF/HWSharedParameter.h"
#include "calibtic/HMF/ModelSharedParameter.h"

namespace HMF {

class SharedCalibration :
	public calibtic::Calibration
{
public:
	SharedCalibration();
	virtual ~SharedCalibration();

	void setDefaults();

	// Py++ factory function
	static boost::shared_ptr<SharedCalibration> create();

	// v_reset in V
	HWSharedParameter applySharedCalibration(double v_reset) const;

	HWSharedParameter applySharedCalibration(
			ModelSharedParameter const & p
			) const;

	// reverse trafo. TODO: discuss, whether this requires a separate class.
	ModelSharedParameter
	applySharedReverse(
			HWSharedParameter const & p
			) const;

	virtual void copy(calibtic::Calibration const&);

	// technical analog -> digital DAC values
	// parameter v in to_dac() must be in SI units, e.g. Ampere, Volt, Second, ...
	int to_dac(double const v, HICANN::shared_parameter const p,
		   trafo_t::OutsideDomainBehavior outside_domain_behavior = trafo_t::CLIP) const;
	static int volt_to_dac(double val);
	static double dac_to_volt(int val);

private:

	typedef HWSharedParameter::value_type hw_value;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};

} // HMF

// implementation
namespace HMF {

template<typename Archiver>
void SharedCalibration::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Calibration);
}

} // HMF

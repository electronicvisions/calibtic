#pragma once
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/array.hpp>

#include <boost/shared_ptr.hpp>

#include "calibtic/Calibration.h"
#include "calibtic/backend/Backend.h"

#include "calibtic/HMF/ADC/VoltageMeasurement.h"

#include "halco/hicann/v2/fwd.h"

#include "pywrap/compat/numpy.hpp"

#include "calibtic/HMF/ADC/ADCCalibration.h"

namespace HMF {
namespace ADC {

// coefficients for polynomial a x^2 + b x + c
struct QuadraticCoefficients
{
	PYPP_INIT(double a, 0.0);
	PYPP_INIT(double b, 0.0);
	PYPP_INIT(double c, 0.0);
};

/// Performat implementation of a ADCCalibration
/// see: tools/compare_adc_calibrations
class QuadraticADCCalibration :
	public ADCCalibration
{
public:
	typedef halco::hicann::v2::ChannelOnADC key_type;
	typedef std::array<QuadraticCoefficients, 8> coefficents_t;

	QuadraticADCCalibration();
	QuadraticADCCalibration(coefficents_t);
	virtual ~QuadraticADCCalibration();

	std::vector<float>
	virtual apply(key_type channel, std::vector<uint16_t> const& data) const PYPP_OVERRIDE;

	pyublas::numpy_vector<float>
	virtual apply(key_type channel, pyublas::numpy_vector<uint16_t> const& data) const PYPP_OVERRIDE;

	virtual std::ostream& operator<< (std::ostream&) const;

	calibtic::MetaData
	load(boost::shared_ptr<calibtic::backend::Backend> backend,
		 HMF::ADC::USBSerial id);

	void
	store(boost::shared_ptr<calibtic::backend::Backend> backend,
		  calibtic::MetaData& md,
		  HMF::ADC::USBSerial id);

	// Py++ factory function
	static
	boost::shared_ptr<QuadraticADCCalibration> create();

	virtual bool isComplete() const PYPP_OVERRIDE;
private:
	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);

	coefficents_t mCoefficents;
};

} // ADC
} // HMF

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, HMF::ADC::QuadraticCoefficients & c, const unsigned int)
{
	using namespace boost::serialization;
	ar & make_nvp("a", c.a) & make_nvp("b", c.b) & make_nvp("c", c.c);
}

} // namespace serialization
} // namespace boost


namespace HMF {
namespace ADC {

template<typename Archiver>
void QuadraticADCCalibration::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ADCCalibration)
	   & make_nvp("coefficents", mCoefficents);

}

} // ADC
} // HMF

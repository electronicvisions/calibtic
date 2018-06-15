#pragma once
#include <boost/optional.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/optional.hpp>

#include <boost/shared_ptr.hpp>

#include "calibtic/Calibration.h"
#include "calibtic/backend/Backend.h"

#include "HMF/ADC/VoltageMeasurement.h"

#include "hal/Coordinate/HMFGeometry.h"
#include "hal/ADC/USBSerial.h"

#include "pywrap/compat/numpy.hpp"
#include "pywrap/compat/macros.hpp"

namespace HMF {
namespace ADC {

class QuadraticADCCalibration;

class ADCCalibration :
	public calibtic::Calibration
{
public:
	typedef Coordinate::ChannelOnADC key_type;

	ADCCalibration();
	virtual ~ADCCalibration();

	std::vector<float>
	virtual apply(key_type channel, std::vector<uint16_t> const& data) const;

	pyublas::numpy_vector<float>
	virtual apply(key_type channel, pyublas::numpy_vector<uint16_t> const& data) const;

	void makePolynomialTrafo(Calibration::key_type offset,
		VoltageMeasurement const& voltage,
		unsigned const order = 2);

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
	boost::shared_ptr<ADCCalibration> create();

	// Returns a canonical calibration, that maps the 16bit DAC values
	// to voltages from 0V to 1.8V
	static ADCCalibration getDefaultCalibration();

    // Returns ESS calibration, that leaves the trace
	// to voltages from 0V to 1.8V
	static ADCCalibration getESSCalibration();

	// Convert to QuadraticADCCalibration
	static
	boost::shared_ptr<QuadraticADCCalibration> convertToQuadraticADCCalibration(
		const ADCCalibration & other);

	virtual bool isComplete() const;

protected:
	ADCCalibration(size_type const size);
	static std::string storage_id(HMF::ADC::USBSerial id);

private:
	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};

boost::shared_ptr<ADCCalibration>
loadADCCalibration(
	boost::shared_ptr<calibtic::backend::Backend> backend,
	calibtic::MetaData & md,
	HMF::ADC::USBSerial id);

void storeADCCalibration(
	boost::shared_ptr<calibtic::backend::Backend> backend,
	boost::shared_ptr<ADCCalibration> calibration,
	calibtic::MetaData md,
	HMF::ADC::USBSerial id);

} // ADC
} // HMF



// implementation
namespace HMF {
namespace ADC {

template<typename Archiver>
void ADCCalibration::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Calibration);
}

} // ADC
} // HMF

#include "HMF/ADC/ADCCalibration.h"

#include <sstream>
#include <memory>
#include <cmath>
#include <boost/make_shared.hpp>
#include <gsl/gsl_multifit.h>
#include <log4cxx/logger.h>

#include "calibtic/trafo/Polynomial.h"
#include "pythonic/zip.h"
#include "hal/Coordinate/iter_all.h"
#include "HMF/ADC/QuadraticADCCalibration.h"

namespace HMF {
namespace ADC {

using namespace pythonic;

ADCCalibration::ADCCalibration() :
	Calibration(8)
{}

ADCCalibration::ADCCalibration(size_type const size) :
	Calibration(size)
{}

ADCCalibration::~ADCCalibration()
{}

calibtic::MetaData ADCCalibration::load(boost::shared_ptr<calibtic::backend::Backend> backend, HMF::ADC::USBSerial id)
{
	if (!backend)
		throw std::runtime_error("ADCCalibration needs a backend to load data.");

	calibtic::MetaData md;
	backend->load(storage_id(id), md, *this);
	if (!isComplete())
	{
		std::stringstream err;
		err << "Loaded ADCCalibration for " << id << " is corrupted\n";
		throw std::runtime_error(err.str());
	}
	return md;
}

void ADCCalibration::store(boost::shared_ptr<calibtic::backend::Backend> backend, calibtic::MetaData& md, HMF::ADC::USBSerial id)
{
	if (!backend)
		throw std::runtime_error("ADCCalibration needs a backend to store data.");
	if (!isComplete())
	{
		std::stringstream err;
		err << "Storing of ADCCalibration for " << id
			<< " aborted. The calibration is incomplete!";
		throw std::runtime_error(err.str());
	}

	backend->store(storage_id(id), md, *this);
}

std::vector<float>
ADCCalibration::apply(key_type channel, std::vector<uint16_t> const& data) const
{
	std::vector<float> voltages (data.size());

	const_value_type trafo;
	if (!isComplete()) {
		throw std::runtime_error("invalid ADC Calibration");
	}

	for(auto const& it : zip(data, voltages)) {
		applyOne(it.first, it.second, channel);
	}
	return voltages;
}

pyublas::numpy_vector<float>
ADCCalibration::apply(key_type channel, pyublas::numpy_vector<uint16_t> const& data) const
{
	pyublas::numpy_vector<float> voltages (data.size());

	const_value_type trafo;
	if (!isComplete()) {
		throw std::runtime_error("invalid ADC Calibration");
	}

	for(auto const& it : zip(data, voltages)) {
		applyOne(it.first, it.second, channel);
	}
	return voltages;
}

void ADCCalibration::makePolynomialTrafo(
	Calibration::key_type offset,
	VoltageMeasurement const& voltage,
	unsigned const order)
{
	// ugly C-style GSL weighted polynomial fitting, for more details see:
	// http://www.gnu.org/software/gsl/manual/html_node/Fitting-Examples.html
	size_t const n = voltage.size();
	size_t const n_coeff = order + 1;

	if (n < n_coeff) {
		throw std::runtime_error(
			"Order of polynomial has to be < #data points.");
	}

	typedef std::shared_ptr<gsl_matrix> mat;
	auto make_matrix = [](size_t n, size_t m) {
		return mat(gsl_matrix_alloc (n, m),
				   [](gsl_matrix* m) { gsl_matrix_free(m); }); };

	typedef std::shared_ptr<gsl_vector> vec;
	auto make_vector = [](size_t n) {
		return vec(gsl_vector_alloc(n),
				   [](gsl_vector* v) { gsl_vector_free(v); }); };

	double chisq;
	mat X = make_matrix(n, n_coeff);
	vec y = make_vector(n);
	vec w = make_vector(n);
	vec c = make_vector(n_coeff);
	mat cov = make_matrix(n_coeff, n_coeff);

	size_t cnt = 0;
	for (auto const& point : voltage.data())
	{
		gsl_matrix_set(X.get(), cnt, 0, 1.0);
		for (size_t ii = 1; ii< n_coeff; ++ii) {
			gsl_matrix_set(X.get(), cnt, ii, pow(point.mean, ii));
		}

		gsl_vector_set(y.get(), cnt, point.ref);

		// weight measurement points with big error less
		gsl_vector_set(w.get(), cnt, 1.0/(pow(point.std, 2)));

		++cnt;
	}

	// do the fit
	gsl_multifit_linear_workspace* work =
		gsl_multifit_linear_alloc(n, n_coeff);
	gsl_multifit_wlinear(X.get(), w.get(), y.get(), c.get(),
						 cov.get(), &chisq, work);
	gsl_multifit_linear_free(work);


	calibtic::trafo::Polynomial::data_type coeff;
	for (size_t ii = 0; ii<n_coeff; ++ii) {
		coeff.push_back(gsl_vector_get(c.get(), (ii)));
	}

	assert(size() == 8);
	reset(offset, calibtic::trafo::Polynomial::create(coeff));
}

std::ostream& ADCCalibration::operator<< (std::ostream& os) const
{
	os << "ADCCalibration: ";
	return Calibration::operator<<(os);
}

// Py++ factory function
boost::shared_ptr<ADCCalibration> ADCCalibration::create()
{
	return boost::shared_ptr<ADCCalibration>(new ADCCalibration());
}

boost::shared_ptr<QuadraticADCCalibration>
ADCCalibration::convertToQuadraticADCCalibration(const ADCCalibration & other)
{
	if (!other.isComplete()) {
		throw std::runtime_error("Cannot convert incomplete ADCCalibration.");
	}

	const char * const err = "Cannot convert ADCCalibration. It does not only"
	                         "uses Polynomials of hightest degree two as "
							 "transformation.";

	QuadraticADCCalibration::coefficents_t coefficents;
	for(auto it : zip(other.mTrafo, coefficents)) {
		boost::shared_ptr<calibtic::trafo::Polynomial> polynomial =
			boost::dynamic_pointer_cast<calibtic::trafo::Polynomial>(it.first);
		if (!polynomial)
		{
			throw std::runtime_error(err);
		}

		QuadraticCoefficients & coeffs = it.second;
		switch (polynomial->degree())
		{
			case 2:
				coeffs.a = polynomial->getData()[2];
				// ECM(2017-12-04): replace by [[fallthrough]];
				/* falls through */
			case 1:
				coeffs.b = polynomial->getData()[1];
				// ECM(2017-12-04): replace by [[fallthrough]];
				/* falls through */
			case 0:
				coeffs.c = polynomial->getData()[0];
				break;
			default:
				throw std::runtime_error(err);
		}
	}
	return boost::make_shared<QuadraticADCCalibration>(coefficents);

}

bool ADCCalibration::isComplete() const
{
	if (size() != static_cast<size_t>(key_type::end))
		return false;
	for (auto channel : Coordinate::iter_all<Coordinate::ChannelOnADC>())
	{
		if (channel != key_type::GND && !exists(channel))
			return false;
	}
	return true;
}

ADCCalibration
ADCCalibration::getDefaultCalibration()
{
	ADCCalibration calib;
	const double a0 = + 1.8;
	const double a1 = - 1.8 / 4095.0;
	for (auto channel : Coordinate::iter_all<Coordinate::ChannelOnADC>())
	{
		if (channel != key_type::GND)
			calib.reset(channel, calibtic::trafo::Polynomial::create({a0, a1}));
	}
	return calib;
}

ADCCalibration
ADCCalibration::getESSCalibration()
{
	ADCCalibration calib;
	const double a0 = 0.;
	const double a1 = 1.8/65535;
	for (auto channel : Coordinate::iter_all<Coordinate::ChannelOnADC>())
	{
		if (channel != key_type::GND)
		    calib.reset(channel, calibtic::trafo::Polynomial::create({a0, a1}));
	}
	return calib;
}

std::string ADCCalibration::storage_id(HMF::ADC::USBSerial id)
{
	std::stringstream sid;
	sid << "adc-" << id.get_serial();
	return sid.str();
}

namespace {
	std::string ADCCalibrationId(HMF::ADC::USBSerial id)
	{
		std::stringstream sid;
		sid << "adc2-" << id.get_serial();
		return sid.str();
	}
}

boost::shared_ptr<ADCCalibration>
loadADCCalibration(
	boost::shared_ptr<calibtic::backend::Backend> backend,
	calibtic::MetaData & md,
	HMF::ADC::USBSerial id)
{
	if (!backend) {
		throw std::runtime_error("Invalid backend.");
	}

	boost::shared_ptr<calibtic::Calibration> tmp;
	backend->load(ADCCalibrationId(id), md, tmp);
	boost::shared_ptr<ADCCalibration> calibration =
		boost::dynamic_pointer_cast<ADCCalibration>(tmp);

	if (!calibration) {
		throw std::runtime_error(
			"The loaded calibration is not an ADCCalbration");
	}

	if (!calibration->isComplete())
	{
		std::stringstream err;
		err << "Loaded ADCCalibration for " << id << " is corrupted\n";
		throw std::runtime_error(err.str());
	}
	return calibration;
}

void storeADCCalibration(
	boost::shared_ptr<calibtic::backend::Backend> backend,
	boost::shared_ptr<ADCCalibration> calibration,
	calibtic::MetaData md,
	HMF::ADC::USBSerial id)
{
	if (!backend) {
		throw std::runtime_error("Invalid backend.");
	}
	if (!calibration->isComplete())
	{
		std::stringstream err;
		err << "Storing of ADCCalibration for " << id
			<< " aborted. The calibration is incomplete!";
		throw std::runtime_error(err.str());
	}

	backend->store(ADCCalibrationId(id), md, calibration);
}

} // ADC
} // HMF

#include "calibtic/HMF/ADC/QuadraticADCCalibration.h"

#include <sstream>
#include <memory>
#include <cmath>
#include <gsl/gsl_multifit.h>

#include "calibtic/trafo/Polynomial.h"
#include "pythonic/zip.h"
#include "halco/common/iter_all.h"
#include "halco/hicann/v2/external.h"

namespace HMF {
namespace ADC {

using namespace pythonic;
using namespace halco::hicann::v2;

QuadraticADCCalibration::QuadraticADCCalibration() :
	ADCCalibration(0)
{
}

QuadraticADCCalibration::QuadraticADCCalibration(coefficents_t coefficents) :
	mCoefficents(coefficents)
{
}

QuadraticADCCalibration::~QuadraticADCCalibration()
{
}

namespace {
	template<typename ResultType, typename InType>
	inline ResultType apply_impl(const QuadraticCoefficients coeffs,
			                     InType const& data)
	{
		ResultType result(data.size());
		typename ResultType::iterator out = result.begin();
		for(auto it = data.begin(); it != data.end(); ++it, ++out)
		{
			typename ResultType::value_type v = *it;
			*out = (coeffs.a * v +  coeffs.b) * v + coeffs.c;
		}
		return result;
	}
}

std::vector<float>
QuadraticADCCalibration::apply(
		key_type channel, std::vector<uint16_t> const& data) const
{
	return apply_impl<std::vector<float>>(mCoefficents.at(channel), data);
}

pyublas::numpy_vector<float>
QuadraticADCCalibration::apply(
		key_type channel, pyublas::numpy_vector<uint16_t> const& data) const
{
	return apply_impl<pyublas::numpy_vector<float>>(mCoefficents.at(channel), data);
}

std::ostream& QuadraticADCCalibration::operator<< (std::ostream& out) const
{
	out << "QuadraticADCCalibration:\n";
	for (coefficents_t::const_iterator it = mCoefficents.begin();
         it != mCoefficents.end(); ++it)
	{
		out << "    " << it->a << " x^2 + " << it->b << " x + " << it->c << "\n";
	}
	return out;
}

calibtic::MetaData
QuadraticADCCalibration::load(
	boost::shared_ptr<calibtic::backend::Backend> backend,
	HMF::ADC::USBSerial id)
{
	if (!backend)
		throw std::runtime_error("QuadraticADCCalibration needs a backend to load data.");

	std::stringstream sid;
	sid << "adc-" << id.get_serial();
	calibtic::MetaData md;
	backend->load(sid.str(), md, *this);
	if (!isComplete())
	{
		std::stringstream err;
		err << "Loaded QuadraticADCCalibration for " << id << " is corrupted\n";
		throw std::runtime_error(err.str());
	}
	return md;
}

void
QuadraticADCCalibration::store(
	boost::shared_ptr<calibtic::backend::Backend> backend,
	calibtic::MetaData& md,
	HMF::ADC::USBSerial id)
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

// Py++ factory function
boost::shared_ptr<QuadraticADCCalibration> QuadraticADCCalibration::create()
{
	return boost::shared_ptr<QuadraticADCCalibration>(new QuadraticADCCalibration());
}

bool QuadraticADCCalibration::isComplete() const
{
	bool complete = true;
	for (coefficents_t::const_iterator it = mCoefficents.begin();
         it != mCoefficents.end(); ++it)
	{
		complete &= (it->a != 0.0 && it->b != 0.0 && it->c != 0.0);
	}
	return complete;
}


} // ADC
} // HMF

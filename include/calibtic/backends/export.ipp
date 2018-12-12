// polymorphic classes need to be registered in each backend
#include "calibtic/Base.h"
#include "calibtic/Collection.h"
#include "calibtic/Calibration.h"
#include "calibtic/MetaData.h"

#include "calibtic/trafo/Transformation.h"
#include "calibtic/trafo/Constant.h"
#include "calibtic/trafo/Polynomial.h"
#include "calibtic/trafo/SumOfTrafos.h"
#include "calibtic/trafo/PowerOfTrafo.h"
#include "calibtic/trafo/OneOverPolynomial.h"
#include "calibtic/trafo/NegativePowersPolynomial.h"
#include "calibtic/trafo/InvQuadraticPol.h"
#include "calibtic/trafo/Lookup.h"

#include "calibtic/HMF/NeuronCalibration.h"
#include "calibtic/HMF/SharedCalibration.h"
#include "calibtic/HMF/NeuronCollection.h"
#include "calibtic/HMF/BlockCollection.h"
#include "calibtic/HMF/HICANNCollection.h"
#include "calibtic/HMF/SynapseRowCollection.h"
#include "calibtic/HMF/SynapseRowCalibration.h"
#include "calibtic/HMF/SynapseCalibration.h"

#include "calibtic/HMF/ADC/ADCCalibration.h"
#include "calibtic/HMF/ADC/QuadraticADCCalibration.h"

BOOST_CLASS_EXPORT(calibtic::trafo::Transformation)
BOOST_CLASS_EXPORT(calibtic::trafo::Constant)
BOOST_CLASS_EXPORT(calibtic::trafo::Polynomial)
BOOST_CLASS_EXPORT(calibtic::trafo::OneOverPolynomial)
BOOST_CLASS_EXPORT(calibtic::trafo::NegativePowersPolynomial)
BOOST_CLASS_EXPORT(calibtic::trafo::InvQuadraticPol)
BOOST_CLASS_EXPORT(calibtic::trafo::SumOfTrafos)
BOOST_CLASS_EXPORT(calibtic::trafo::PowerOfTrafo)
BOOST_CLASS_EXPORT(calibtic::trafo::Lookup)


// base stuff
BOOST_CLASS_EXPORT(calibtic::Base)
BOOST_CLASS_EXPORT(calibtic::Collection)
BOOST_CLASS_EXPORT(calibtic::Calibration)
BOOST_CLASS_EXPORT(calibtic::MetaData)

// HMF
BOOST_CLASS_EXPORT(HMF::NeuronCalibration)
BOOST_CLASS_EXPORT(HMF::SharedCalibration)
BOOST_CLASS_EXPORT(HMF::NeuronCollection)
BOOST_CLASS_EXPORT(HMF::BlockCollection)
BOOST_CLASS_EXPORT(HMF::HICANNCollection)
BOOST_CLASS_EXPORT(HMF::SynapseRowCollection)
BOOST_CLASS_EXPORT(HMF::SynapseCalibration)
BOOST_CLASS_EXPORT(HMF::SynapseRowCalibration)

BOOST_CLASS_EXPORT(HMF::ADC::ADCCalibration)
BOOST_CLASS_EXPORT(HMF::ADC::QuadraticADCCalibration)

#include <boost/serialization/void_cast.hpp>

static int __void_cast_register()
{
	using namespace boost::serialization;
	using namespace calibtic;

	// calibtic
	void_cast_register<Collection, Base>();
	void_cast_register<Calibration, Base>();

	// HMF
	void_cast_register<HMF::NeuronCollection, Collection>();
	void_cast_register<HMF::BlockCollection, Collection>();
	void_cast_register<HMF::HICANNCollection, Collection>();
	void_cast_register<HMF::SynapseRowCollection, Collection>();
	void_cast_register<HMF::SynapseRowCalibration, Base>();
	void_cast_register<HMF::SynapseCalibration, Calibration>();
	void_cast_register<HMF::NeuronCalibration, Calibration>();
	void_cast_register<HMF::SharedCalibration, Calibration>();

	void_cast_register<HMF::ADC::ADCCalibration, Calibration>();
	void_cast_register<HMF::ADC::QuadraticADCCalibration, Calibration>();

	return 0;
}

static int const __dummy __attribute__((unused)) = __void_cast_register();

#define CALIBTIC_BACKEND_EXPORT

#include <gtest/gtest.h>

#include <sstream>
#include <fstream>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

// export.ipp must be included to provide correct serialization, but can only
// be inlcude once per linkage unit, see export.ipp, bindings.h and
// BOOST_CLASS_EXPORT documantaion for details.
#include <boost/serialization/export.hpp>
#undef BOOST_CLASS_EXPORT
#define BOOST_CLASS_EXPORT(T)  BOOST_CLASS_EXPORT_KEY(T)
#include "calibtic/backends/export.ipp"
#include <boost/serialization/export.hpp>

using namespace std;
using namespace calibtic;
using namespace calibtic::trafo;
using namespace HMF;

template<typename T>
class CalibticTypes: public ::testing::Test
{};

typedef ::testing::Types<
	Polynomial,
	Constant,
	InvQuadraticPol,
	SumOfTrafos,
	PowerOfTrafo,
	Lookup,
	Calibration,
	Collection,
	NeuronCalibration,
	BlockCollection,
	HICANNCollection,
	NeuronCollection,
	SharedCalibration,
	SynapseCalibration,
	GmaxConfig,
	SynapseRowCalibration,
	SynapseRowCollection
> ContainerTypes;
TYPED_TEST_SUITE(CalibticTypes, ContainerTypes);

// TODO: test other backends
TYPED_TEST(CalibticTypes, HasSerialization) {
	TypeParam obj, obj2;

	std::stringstream stream;
	{
		boost::archive::xml_oarchive oa(stream);
		oa << boost::serialization::make_nvp("obj", obj);
	}
	stream.flush();
	{
		boost::archive::xml_iarchive ia(stream);
		ia >> boost::serialization::make_nvp("obj", obj2);
	}

	ASSERT_EQ(obj2, obj);
}

typedef ::testing::Types<
	Polynomial,
	Constant,
	InvQuadraticPol,
	SumOfTrafos,
	PowerOfTrafo,
	Lookup
> TrafoTypes;
TYPED_TEST_SUITE(TrafoSerialization, TrafoTypes);

template<typename T>
class TrafoSerialization : public ::testing::Test
{};

// TODO: test other backends
TYPED_TEST(TrafoSerialization, HasSerialization) {
	TypeParam obj, obj2;

	obj.setDomain(-3,3);

	std::stringstream stream;
	{
		boost::archive::xml_oarchive oa(stream);
		oa << boost::serialization::make_nvp("obj", obj);
	}
	stream.flush();
	{
		boost::archive::xml_iarchive ia(stream);
		ia >> boost::serialization::make_nvp("obj", obj2);
	}

	ASSERT_EQ(obj2, obj);
}

typedef ::testing::Types<
	Calibration
	, NeuronCalibration
	, ADC::ADCCalibration
	, ADC::QuadraticADCCalibration
> CalibrationTypes;

template<typename T>
class CalibticCalibrationSerialization : public ::testing::Test
{};

TYPED_TEST_SUITE(CalibticCalibrationSerialization, CalibrationTypes);

TYPED_TEST(CalibticCalibrationSerialization, TestBasePointerSerialization) {
	boost::shared_ptr< TypeParam > original(new TypeParam());
	boost::shared_ptr< Calibration > loaded;
	TypeParam obj, obj2;

	std::stringstream stream;
	{
		boost::archive::xml_oarchive oa(stream);
		boost::shared_ptr< Calibration > tmp =
			boost::dynamic_pointer_cast<Calibration>(original);
		oa << boost::serialization::make_nvp("obj", tmp);
	}
	stream.flush();
	{
		boost::archive::xml_iarchive ia(stream);
		ia >> boost::serialization::make_nvp("obj", loaded);
	}

	ASSERT_TRUE(static_cast<bool>(loaded));
	ASSERT_EQ(*loaded, *original);
}

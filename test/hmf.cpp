#include "test/test.h"

#include "HMF/NeuronCalibration.h"
#include "HMF/NeuronCollection.h"
#include "HMF/HICANNCollection.h"
#include "HMF/SharedCalibration.h"
#include "HMF/BlockCollection.h"
#include "HMF/SynapseRowCalibration.h"
#include "HMF/SynapseRowCollection.h"

using namespace HMF;

template <typename T>
class HMFCalibrationObject : public ::testing::Test {};

typedef ::testing::Types<
	NeuronCalibration,
	NeuronCollection,
	HICANNCollection,
	SharedCalibration,
	BlockCollection,
	SynapseCalibration,
	SynapseRowCalibration,
	SynapseRowCollection
	> TestedHMFObjects;
TYPED_TEST_CASE(HMFCalibrationObject, TestedHMFObjects);

TYPED_TEST(HMFCalibrationObject, DefaultsCanBeSetTwice)
{
	TypeParam obj;
	obj.setDefaults();
	obj.setDefaults();
}

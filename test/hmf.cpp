#include "test.h"

#include "calibtic/HMF/NeuronCalibration.h"
#include "calibtic/HMF/NeuronCollection.h"
#include "calibtic/HMF/HICANNCollection.h"
#include "calibtic/HMF/SharedCalibration.h"
#include "calibtic/HMF/BlockCollection.h"
#include "calibtic/HMF/SynapseRowCalibration.h"
#include "calibtic/HMF/SynapseRowCollection.h"

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

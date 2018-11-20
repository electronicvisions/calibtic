#include "test.h"

#include "calibtic/HMF/NeuronCalibration.h"
#include "calibtic/HMF/NeuronCollection.h"
#include "calibtic/HMF/HICANNCollection.h"
#include "calibtic/HMF/SharedCalibration.h"
#include "calibtic/HMF/BlockCollection.h"
#include "calibtic/HMF/SynapseRowCalibration.h"
#include "calibtic/HMF/SynapseRowCollection.h"
#include "calibtic/HMF/L1CrossbarCollection.h"
#include "calibtic/HMF/L1CrossbarCalibration.h"
#include "calibtic/HMF/SynapseChainLengthCollection.h"
#include "calibtic/HMF/SynapseChainLengthCalibration.h"
#include "calibtic/HMF/SynapseSwitchCalibration.h"
#include "calibtic/HMF/SynapseSwitchCollection.h"

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
    SynapseRowCollection,
    L1CrossbarCollection,
    L1CrossbarCalibration,
    SynapseChainLengthCollection,
    SynapseChainLengthCalibration,
    SynapseSwitchCollection,
    SynapseSwitchCalibration>
    TestedHMFObjects;
TYPED_TEST_CASE(HMFCalibrationObject, TestedHMFObjects);

TYPED_TEST(HMFCalibrationObject, DefaultsCanBeSetTwice)
{
	TypeParam obj;
	obj.setDefaults();
	obj.setDefaults();
}

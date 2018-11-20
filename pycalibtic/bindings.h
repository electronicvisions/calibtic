#pragma once

// Calibtic base
#include "calibtic/Collection.h"
#include "calibtic/Calibration.h"
#include "calibtic/MetaData.h"

// Calibtic Transformastions
#include "calibtic/trafo/Polynomial.h"
#include "calibtic/trafo/OneOverPolynomial.h"
#include "calibtic/trafo/NegativePowersPolynomial.h"
#include "calibtic/trafo/Constant.h"
#include "calibtic/trafo/SumOfTrafos.h"
#include "calibtic/trafo/PowerOfTrafo.h"
#include "calibtic/trafo/Lookup.h"

// Calibtic Backends
#include "calibtic/backend/Library.h"
#include "calibtic/backend/Backend.h"

// HMF stuff
#include "calibtic/HMF/NeuronCollection.h"
#include "calibtic/HMF/BlockCollection.h"
#include "calibtic/HMF/HICANNCollection.h"
#include "calibtic/HMF/NeuronCalibration.h"
#include "calibtic/HMF/SharedCalibration.h"
#include "calibtic/HMF/HWNeuronParameter.h"
#include "calibtic/HMF/HWSharedParameter.h"
#include "calibtic/HMF/ADC/ADCCalibration.h"
#include "calibtic/HMF/SynapseCalibration.h"
#include "calibtic/HMF/GmaxConfig.h"
#include "calibtic/HMF/SynapseRowCalibration.h"
#include "calibtic/HMF/SynapseRowCollection.h"
#include "calibtic/HMF/STPUtilizationCalibration.h"
#include "calibtic/HMF/L1CrossbarCollection.h"
#include "calibtic/HMF/L1CrossbarCalibration.h"
#include "calibtic/HMF/SynapseChainLengthCollection.h"
#include "calibtic/HMF/SynapseChainLengthCalibration.h"
#include "calibtic/HMF/SynapseSwitchCalibration.h"
#include "calibtic/HMF/SynapseSwitchCollection.h"

/// Workaround for pickle support:
/// We use the factory design pattern for class construction which implies that we handle only
/// pointers. BOOST SERIALIZATION documentation says:
///     Hence, the need for export is implied by the usage of a derived class
///     that is manipulated via a pointer or reference to its base class.
///     [ http://www.boost.org/doc/libs/1_49_0/libs/serialization/doc/traits.html#export ]
/// BOOST_CLASS_EXPORT creates a static instance that will be created multiple
/// times for single class. To avoid this we have to use the splited variant
/// BOOST_CLASS_EXPORT_KEY and BOOST_CLASS_EXPORT_IMPLEMENT.
/// We add BOOST_CLASS_EXPORT_IMPLEMENT in generate.py and overwrite
/// BOOST_CLASS_EXPORT to BOOST_CLASS_EXPORT_KEY for the export.ipp header
#include <boost/serialization/export.hpp>
#undef BOOST_CLASS_EXPORT
#define BOOST_CLASS_EXPORT(T)  BOOST_CLASS_EXPORT_KEY(T)
#include "calibtic/backends/export.ipp"
#include <boost/serialization/export.hpp>

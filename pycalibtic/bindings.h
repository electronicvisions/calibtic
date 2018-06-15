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
#include "HMF/NeuronCollection.h"
#include "HMF/BlockCollection.h"
#include "HMF/HICANNCollection.h"
#include "HMF/NeuronCalibration.h"
#include "HMF/SharedCalibration.h"
#include "HMF/HWNeuronParameter.h"
#include "HMF/HWSharedParameter.h"
#include "HMF/ADC/ADCCalibration.h"
#include "HMF/SynapseCalibration.h"
#include "HMF/GmaxConfig.h"
#include "HMF/SynapseRowCalibration.h"
#include "HMF/SynapseRowCollection.h"
#include "HMF/STPUtilizationCalibration.h"

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
#include "backends/export.ipp"
#include <boost/serialization/export.hpp>

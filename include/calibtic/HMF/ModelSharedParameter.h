#pragma once

#include <functional>
#include <string>
#include <map>
#include "euter/parameter.h"

// This uses automatic code generation from euter/cellparameters.h

namespace HMF{

PARAMETER_STRUCT(ModelSharedParameter,
	// Neurons
	PARAMETER(double, v_reset, 0.5), // Volt

	// STP, cf. e.g. Bachelor Thesis by S. Billaudelle
	PARAMETER(double, tau_rec, 10.), // us (corresponds to 100 ms default value of tau_rec in TsodyksMarkramMechanism at 10^4 speedp)
	PARAMETER(double, lambda, 1.0), // unitless
	PARAMETER(double, N_dep, 0.0), // unitless
	PARAMETER(double, N_fac, 1.0) // unitless
)

} // namespace HMF

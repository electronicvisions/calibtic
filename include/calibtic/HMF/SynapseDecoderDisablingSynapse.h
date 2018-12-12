#pragma once

#include "hal/HICANN/SynapseDecoder.h"

/// Defines the 4-bit synapse Address used to disable a synapse.
/// we provide both a raw value (needed for debug in ESS and in mapping tool), as well as a SynapseDecoder instance.
namespace HMF {
namespace HICANN {
	enum  {
		four_bit_address_disabling_synapse = 1 //!< the 4-bit address, that disables a synapse.
	};
	extern SynapseDecoder const SynapseDecoderDisablingSynapse;
} // HICANN
} // HMF


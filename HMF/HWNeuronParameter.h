#pragma once

#include <cstdlib>
#include <vector>
#include "hal/Coordinate/HMFGeometry.h"
#include "hal/HICANN/FGBlock.h" // for parameter enums
#include "hal/HICANN/FGControl.h"

namespace HMF {

namespace HICANN {
enum neuron_parameter; // fwd
} // HICANN


// TODO why not use neuron_parameter in interface?
class HWNeuronParameter
{
public:
	typedef int value_type;

	HWNeuronParameter();
	HWNeuronParameter(std::vector<value_type> const& param);

	static size_t size();

	value_type getParam(int const ii) const;
	void setParam(int const ii, value_type const val);

#ifndef PYPLUSPLUS
	std::vector<value_type> const& parameters() const;
	std::vector<value_type>&       parameters();
#endif

	void toHW(HMF::Coordinate::NeuronOnHICANN const& n, HICANN::FGControl& fg) const;
	void fromHW(HMF::Coordinate::NeuronOnHICANN const& n, HICANN::FGControl const& fg);

private:
	std::vector<value_type> mParam;
};

} // HMF

#ifndef PYPLUSPLUS
namespace HMF {
std::ostream& operator<<(std::ostream& os, HMF::HWNeuronParameter const& p);
}
#endif

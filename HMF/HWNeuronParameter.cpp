#include "HMF/HWNeuronParameter.h"

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.h>
#include <boost/preprocessor/cat.hpp>

// halbe api
#include "hal/HICANN/FGControl.h"
#include "logger.h"

#define HICANN_PARAM_SEQ BOOST_PP_VARIADIC_TO_SEQ(HICANN_PARAM_VARIADIC)

namespace HMF {

HWNeuronParameter::HWNeuronParameter() :
	mParam(size())
{}

HWNeuronParameter::HWNeuronParameter(std::vector<value_type> const& param) :
	mParam(param)
{
	if (size() != param.size()) {
		throw std::runtime_error("wrong number of hw parameters");
	}
}

HWNeuronParameter::value_type
HWNeuronParameter::getParam(int const ii) const
{
	return mParam.at(ii);
}

void HWNeuronParameter::setParam(int const ii, value_type const val)
{
	mParam.at(ii) = val;
}

std::vector<HWNeuronParameter::value_type> const&
HWNeuronParameter::parameters() const
{
	return mParam;
}

std::vector<HWNeuronParameter::value_type>&
HWNeuronParameter::parameters()
{
	return mParam;
}

size_t HWNeuronParameter::size()
{
	return HICANN::neuron_parameter::__last_neuron;
}

void HWNeuronParameter::toHW(HMF::Coordinate::NeuronOnHICANN const& n, HICANN::FGControl& fg) const
{
	auto& log = Logger::instance();

	// fill floating gates from internal parameter vector
	using HMF::HICANN::neuron_parameter;
	for (int pp=0; pp<neuron_parameter::__last_neuron; ++pp)
	{
		value_type val = mParam.at(pp);
		if (val > 1023) {
			log(Logger::WARNING) << "clipping parameter " << pp << " from " << val << " to 1023";
			val = 1023;
		} else if (val < 0) {
			log(Logger::WARNING) << "clipping parameter " << pp << " from " << val << " to 0";
			val = 0;
		}
		fg.setNeuron(n, neuron_parameter(pp), val);
	}
}

void HWNeuronParameter::fromHW(HMF::Coordinate::NeuronOnHICANN const& n, HICANN::FGControl const& fg)
{
	using HMF::HICANN::neuron_parameter;
	for (int pp=0; pp<neuron_parameter::__last_neuron; ++pp)
		mParam.at(pp) = fg.getNeuron(n, neuron_parameter(pp));
}

} // HMF

namespace HMF {
std::ostream& operator<<(std::ostream& os, HMF::HWNeuronParameter const& p)
{
	auto const& v = p.parameters();

	static std::array<std::string, 23> const names {{
		"E_l", "E_syni", "E_synx", "I_bexp", "I_convi",
		"I_convx", "I_fire", "I_gl", "I_gladapt", "I_intbbi",
		"I_intbbx", "I_pl", "I_radapt", "I_rexp", "I_spikeamp",
		"V_exp", "V_syni", "V_syntci", "V_syntcx", "V_synx", "V_t",
		"V_convoffi", "V_convoffx"
	}};

	for (size_t ii=0; ii<v.size(); ++ii)
	{
		os << names.at(ii) << "\t" << v[ii] << '\n';
	}
	return os;
}
}


#undef HICANN_PARAM_SEQ

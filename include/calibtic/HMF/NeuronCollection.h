#pragma once

#include "calibtic/Collection.h"
#include "calibtic/HMF/NeuronCalibration.h"
#include "calibtic/HMF/HWNeuronParameter.h"
#include <iostream>

namespace HMF {

class NeuronCollection :
	public calibtic::Collection
{
public:
	NeuronCollection();
	virtual ~NeuronCollection();

	void setDefaults();

#ifndef PYPLUSPLUS
	HWNeuronParameter applyNeuronCalibration(
		PyNNParameters::EIF_cond_exp_isfa_ista const& model_params,
		size_t const hw_neuron_id,
		NeuronCalibrationParameters const& = {}) const;

	HWNeuronParameter applyNeuronCalibration(
		PyNNParameters::IF_cond_exp const& model_params,
		size_t const hw_neuron_id,
		NeuronCalibrationParameters const& = {}) const;
#else
	HWNeuronParameter applyNeuronCalibration(
		PyNNParameters::EIF_cond_exp_isfa_ista const& model_params,
		size_t const hw_neuron_id,
		NeuronCalibrationParameters const&) const;

	HWNeuronParameter applyNeuronCalibration(
		PyNNParameters::IF_cond_exp const& model_params,
		size_t const hw_neuron_id,
		NeuronCalibrationParameters const&) const;
#endif

	size_t getSpeedup() const;
	void setSpeedup(size_t const s);

	/// clock is pll_frequency/4
	size_t getClock() const;

	size_t getPLLFrequency() const;
	void setPLLFrequency(size_t const freq);

	size_t getStartingCycle() const;
	void setStartingCycle(size_t const sc);

	virtual std::ostream& operator<< (std::ostream&) const;

	// Py++ factory function
	static
	boost::shared_ptr<NeuronCollection> create();

	virtual void copy(Collection const& rhs);

private:
	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);

	template<typename CellType>
	HWNeuronParameter _applyNeuronCalibration(
		CellType const& model_params,
		size_t const hw_neuron_id,
		NeuronCalibrationParameters const&) const;

	size_t mSpeedup;
	size_t mPLLFrequency;
	size_t mStartingCycle;
};

} // HMF


// implementations

namespace HMF {

template<typename Archiver>
void NeuronCollection::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Collection)
	   & make_nvp("speedup", mSpeedup)
	   & make_nvp(    "pll", mPLLFrequency)
	   & make_nvp(  "start", mStartingCycle);
}

template<typename CellType>
HWNeuronParameter NeuronCollection::_applyNeuronCalibration(
	CellType const& model_params,
	size_t const hw_neuron_id,
	NeuronCalibrationParameters const& params) const
{
	if (!exists(hw_neuron_id) || !at(hw_neuron_id)) {
		throw std::runtime_error("no calibration data for this neuron");
	}

	NeuronCalibration const& calib =
		*boost::dynamic_pointer_cast<NeuronCalibration const>(at(hw_neuron_id));

    return calib.applyNeuronCalibration(model_params, mSpeedup, params);
}

} // HMF

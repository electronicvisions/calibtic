#pragma once

#include <cmath>
#include <iostream>
#include <vector>
#include "calibtic/Collection.h"
#include "calibtic/HMF/HICANNCollection.h"
#include "calibtic/HMF/NeuronCollection.h"
#include "calibtic/HMF/BlockCollection.h"
#include "calibtic/HMF/SynapseRowCollection.h"
#include "calibtic/HMF/L1CrossbarCollection.h"
#include "calibtic/HMF/SynapseChainLengthCollection.h"
#include "calibtic/HMF/SynapseRowCollection.h"
#include "calibtic/HMF/SynapseSwitchCollection.h"

namespace HMF {

class HICANNCollection :
	public calibtic::Collection
{
public:
	HICANNCollection();
	virtual ~HICANNCollection();

	void init_missing();
	void setDefaults();

	// FIXME these function should return FGControl
	/*
	HWNeuronParameter applyHICANNCalibration(
		PyNNParameters::EIF_cond_exp_isfa_ista const& param,
		size_t const hw_neuron_id) const;

	HWNeuronParameter applyHICANNCalibration(
		PyNNParameters::IF_cond_exp const& param,
		size_t const hw_neuron_id) const;

	FGControl applyHICANNCalibration(FGControl const& fgvalues, size_t const hw_neuron_id);
	*/

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
	boost::shared_ptr<HICANNCollection> create();

	boost::shared_ptr<NeuronCollection> atNeuronCollection();
	boost::shared_ptr<BlockCollection> atBlockCollection();
	boost::shared_ptr<SynapseRowCollection> atSynapseRowCollection();
	boost::shared_ptr<L1CrossbarCollection> atL1CrossbarCollection();
	boost::shared_ptr<SynapseChainLengthCollection> atSynapseChainLengthCollection();
	boost::shared_ptr<SynapseSwitchCollection> atSynapseSwitchCollection();

#ifndef PYPLUSPLUS
	const boost::shared_ptr<const NeuronCollection> atNeuronCollection() const;

	const boost::shared_ptr<const BlockCollection> atBlockCollection() const;

	const boost::shared_ptr<const SynapseRowCollection> atSynapseRowCollection() const;

	const boost::shared_ptr<const L1CrossbarCollection> atL1CrossbarCollection() const;

	const boost::shared_ptr<const SynapseChainLengthCollection> atSynapseChainLengthCollection() const;

	const boost::shared_ptr<const SynapseSwitchCollection> atSynapseSwitchCollection() const;
#endif

	virtual void copy(Collection const& rhs);

private:
	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);

	/* FIXME
	template<typename CellType>
	HWNeuronParameter _applyHICANNCalibration(
		CellType const& param,
		size_t const hw_neuron_id) const;
	*/

	size_t mSpeedup;
	size_t mPLLFrequency;
	size_t mStartingCycle;

	struct Collection_ID {
		enum
		{
			Neuron = 0,
			Block = 1,
			SynapseRow = 2,
			L1Crossbar = 3,
			SynapseChainLength = 4,
			SynapseSwitches = 5,
		};
	};

};

} // HMF


// implementations

namespace HMF {

template<typename Archiver>
void HICANNCollection::serialize(Archiver& ar, unsigned int const version)
{
	using namespace boost::serialization;
	if (version < 1)
	{
		// old serialization without ESS
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Collection)
		   & make_nvp("speedup", mSpeedup)
		   & make_nvp(    "pll", mPLLFrequency)
		   & make_nvp(  "start", mStartingCycle);
	}
	else if (version < 2)
	{
		bool dummy = false;

		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Collection)
		   & make_nvp("speedup", mSpeedup)
		   & make_nvp(    "pll", mPLLFrequency)
		   & make_nvp(  "start", mStartingCycle)
		   & make_nvp(  "ESS", dummy);
	}
	else
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Collection)
		   & make_nvp("speedup", mSpeedup)
		   & make_nvp(    "pll", mPLLFrequency)
		   & make_nvp(  "start", mStartingCycle);
	}

	init_missing(); // if new contents are not supported by file, do not override them,
	                // but use defaults
}

/* FIXME copied from NeuronCalibration
template<typename CellType>
HWNeuronParameter HICANNCollection::_applyHICANNCalibration(
	CellType const& param,
	size_t const hw_neuron_id) const
{
	if (!exists(hw_neuron_id) || !at(hw_neuron_id)) {
		throw std::runtime_error("no calibration data for this neuron");
	}

	NeuronCalibration const& calib =
		*boost::dynamic_pointer_cast<NeuronCalibration const>(at(hw_neuron_id));

	return calib.applyNeuronCalibration(param, mSpeedup);
}
*/

} // HMF

BOOST_CLASS_VERSION(::HMF::HICANNCollection, 2)

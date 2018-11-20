#include "calibtic/HMF/HICANNCollection.h"

namespace HMF {

HICANNCollection::HICANNCollection()
{

	mSpeedup       = 10000;
	mPLLFrequency  = 125 * pow(10, 6);
	mStartingCycle = 0;

	boost::shared_ptr<NeuronCollection> nc(new NeuronCollection);
	boost::shared_ptr<BlockCollection> bc(new BlockCollection);
	boost::shared_ptr<SynapseRowCollection> sc(new SynapseRowCollection);
	boost::shared_ptr<L1CrossbarCollection> cc(new L1CrossbarCollection);
	boost::shared_ptr<SynapseChainLengthCollection> scl(new SynapseChainLengthCollection);
	boost::shared_ptr<SynapseSwitchCollection> ss(new SynapseSwitchCollection);

	erase(Collection_ID::Neuron);
	insert(Collection_ID::Neuron, nc);
	erase(Collection_ID::Block);
	insert(Collection_ID::Block, bc);
	erase(Collection_ID::SynapseRow);
	insert(Collection_ID::SynapseRow, sc);
	erase(Collection_ID::L1Crossbar);
	insert(Collection_ID::L1Crossbar, cc);
	erase(Collection_ID::SynapseChainLength);
	insert(Collection_ID::SynapseChainLength, scl);
	erase(Collection_ID::SynapseSwitches);
	insert(Collection_ID::SynapseSwitches, ss);
}

HICANNCollection::~HICANNCollection()
{
}

void HICANNCollection::setDefaults()
{
	atNeuronCollection()->setDefaults();
	atBlockCollection()->setDefaults();
	atSynapseRowCollection()->setDefaults();
	atL1CrossbarCollection()->setDefaults();
	atSynapseChainLengthCollection()->setDefaults();
	atSynapseSwitchCollection()->setDefaults();
}

boost::shared_ptr<NeuronCollection> HICANNCollection::atNeuronCollection()
{

	if(!exists(Collection_ID::Neuron)) {
		throw std::runtime_error("HICANNCollection: missing neuron collection");
	}

    boost::shared_ptr<NeuronCollection> nc = boost::dynamic_pointer_cast<NeuronCollection>(
			at(Collection_ID::Neuron)
	);
    return nc;
}

boost::shared_ptr<BlockCollection> HICANNCollection::atBlockCollection()
{

	if(!exists(Collection_ID::Block)) {
		throw std::runtime_error("HICANNCollection: missing block collection");
	}

	return boost::dynamic_pointer_cast<BlockCollection>(
			at(Collection_ID::Block)
	);
}

boost::shared_ptr<SynapseRowCollection> HICANNCollection::atSynapseRowCollection()
{

	if(!exists(Collection_ID::SynapseRow)) {
		throw std::runtime_error("HICANNCollection: missing synapse row collection");
	}

	return boost::dynamic_pointer_cast<SynapseRowCollection>(
			at(Collection_ID::SynapseRow)
	);
}

boost::shared_ptr<L1CrossbarCollection> HICANNCollection::atL1CrossbarCollection()
{
	if (!exists(Collection_ID::L1Crossbar)) {
		throw std::runtime_error("HICANNCollection: missing L1crossbar collection");
	}

	return boost::dynamic_pointer_cast<L1CrossbarCollection>(at(Collection_ID::L1Crossbar));
}

boost::shared_ptr<SynapseChainLengthCollection> HICANNCollection::atSynapseChainLengthCollection()
{
	if (!exists(Collection_ID::SynapseChainLength)) {
		throw std::runtime_error("HICANNCollection: missing SynapseChainLength collection");
	}

	return boost::dynamic_pointer_cast<SynapseChainLengthCollection>(
	    at(Collection_ID::SynapseChainLength));
}

boost::shared_ptr<SynapseSwitchCollection> HICANNCollection::atSynapseSwitchCollection()
{
	if (!exists(Collection_ID::SynapseSwitches)) {
		throw std::runtime_error("HICANNCollection: missing SynapseSwitch collection");
	}

	return boost::dynamic_pointer_cast<SynapseSwitchCollection>(at(Collection_ID::SynapseSwitches));
}

#ifndef PYPLUSPLUS
const boost::shared_ptr<const NeuronCollection> HICANNCollection::atNeuronCollection() const {

	if(!exists(Collection_ID::Neuron)) {
		throw std::runtime_error("HICANNCollection: missing neuron collection");
	}

	const boost::shared_ptr<const NeuronCollection> nc = boost::dynamic_pointer_cast<const NeuronCollection>(
            at(Collection_ID::Neuron)
    );
	return nc;
}

const boost::shared_ptr<const BlockCollection> HICANNCollection::atBlockCollection() const {

	if(!exists(Collection_ID::Block)) {
		throw std::runtime_error("HICANNCollection: missing block collection");
	}

	const boost::shared_ptr<const BlockCollection> bc = boost::dynamic_pointer_cast<const BlockCollection>(at(Collection_ID::Block));
	return bc;
}

const boost::shared_ptr<const SynapseRowCollection> HICANNCollection::atSynapseRowCollection() const {

	if(!exists(Collection_ID::SynapseRow)) {
		throw std::runtime_error("HICANNCollection: missing synapse row collection");
	}

	const boost::shared_ptr<const SynapseRowCollection> sc =
	    boost::dynamic_pointer_cast<const SynapseRowCollection>(at(Collection_ID::SynapseRow));
	return sc;
}

const boost::shared_ptr<const L1CrossbarCollection> HICANNCollection::atL1CrossbarCollection() const
{
	if (!exists(Collection_ID::L1Crossbar)) {
		throw std::runtime_error("HICANNCollection: missing L1Crossbar collection");
	}

	const boost::shared_ptr<const L1CrossbarCollection> cc =
	    boost::dynamic_pointer_cast<const L1CrossbarCollection>(at(Collection_ID::L1Crossbar));
	return cc;
}

const boost::shared_ptr<const SynapseChainLengthCollection>
HICANNCollection::atSynapseChainLengthCollection() const
{
	if (!exists(Collection_ID::SynapseChainLength)) {
		throw std::runtime_error("HICANNCollection: missing synapseChainLength collection");
	}

	const boost::shared_ptr<const SynapseChainLengthCollection> cc =
	    boost::dynamic_pointer_cast<const SynapseChainLengthCollection>(
	        at(Collection_ID::SynapseChainLength));
	return cc;
}

const boost::shared_ptr<const SynapseSwitchCollection> HICANNCollection::atSynapseSwitchCollection()
    const
{
	if (!exists(Collection_ID::SynapseSwitches)) {
		throw std::runtime_error("HICANNCollection: missing synapseSwitch collection");
	}

	const boost::shared_ptr<const SynapseSwitchCollection> ss =
	    boost::dynamic_pointer_cast<const SynapseSwitchCollection>(
	        at(Collection_ID::SynapseSwitches));
	return ss;
}

#endif

/*
HWNeuronParameter HICANNCollection::applyNeuronCalibration(
	PyNNParameters::EIF_cond_exp_isfa_ista const& param,
	size_t const hw_neuron_id) const
{
	return _applyNeuronCalibration(param, hw_neuron_id);
}

HWNeuronParameter HICANNCollection::applyNeuronCalibration(
	PyNNParameters::IF_cond_exp const& param,
	size_t const hw_neuron_id) const
{
	return _applyNeuronCalibration(param, hw_neuron_id);
}
*/

size_t HICANNCollection::getSpeedup() const
{
	return mSpeedup;
}

void HICANNCollection::setSpeedup(size_t const s)
{
	mSpeedup = s;
}

size_t HICANNCollection::getClock() const
{
	return mPLLFrequency / 4;
}

size_t HICANNCollection::getPLLFrequency() const
{
	return mPLLFrequency;
}

void HICANNCollection::setPLLFrequency(size_t const freq)
{
	mPLLFrequency = freq;
}

size_t HICANNCollection::getStartingCycle() const
{
	return mStartingCycle;
}

void HICANNCollection::setStartingCycle(size_t const sc)
{
	mStartingCycle = sc;
}

boost::shared_ptr<HICANNCollection> HICANNCollection::create()
{
	return boost::shared_ptr<HICANNCollection>(
		new HICANNCollection());
}

void HICANNCollection::copy(Collection const& rhs)
{
	*this = dynamic_cast<HICANNCollection const&>(rhs);
}

std::ostream& HICANNCollection::operator<< (std::ostream& os) const
{
	os << "HICANNCollection:"
		<< " speedup: " << mSpeedup
		<< " pll: " << mPLLFrequency;
	return Collection::operator<<(os);
}

} // HMF

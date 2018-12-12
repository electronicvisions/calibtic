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

	erase(Collection_ID::Neuron);
	insert(Collection_ID::Neuron, nc);
	erase(Collection_ID::Block);
	insert(Collection_ID::Block, bc);
	erase(Collection_ID::SynapseRow);
	insert(Collection_ID::SynapseRow, sc);

}

HICANNCollection::~HICANNCollection()
{
}

void HICANNCollection::setDefaults()
{
	atNeuronCollection()->setDefaults();
	atBlockCollection()->setDefaults();
	atSynapseRowCollection()->setDefaults();
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

	const boost::shared_ptr<const SynapseRowCollection> sc = boost::dynamic_pointer_cast<const SynapseRowCollection>(at(Collection_ID::SynapseRow));
	return sc;
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

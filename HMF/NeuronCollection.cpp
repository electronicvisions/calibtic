#include "HMF/NeuronCollection.h"
#include "HMF/NeuronCalibration.h"
#include <cmath>
#include <vector>

namespace HMF {

NeuronCollection::NeuronCollection() :
	mSpeedup(10000),
	mPLLFrequency(100*pow(10, 6)),
	mStartingCycle(0)
{}

NeuronCollection::~NeuronCollection()
{}

void NeuronCollection::setDefaults()
{
	mSpeedup       = 10000;
	mPLLFrequency  = 100 * pow(10, 6);
	mStartingCycle = 1000;

	boost::shared_ptr<NeuronCalibration> nc(new NeuronCalibration);
	nc->setDefaults();
	for (size_t ii = 0; ii<HMF::Coordinate::NeuronOnHICANN::enum_type::size; ++ii) {
		erase(ii);
		insert(ii, nc);
	}
}

HWNeuronParameter NeuronCollection::applyNeuronCalibration(
	PyNNParameters::EIF_cond_exp_isfa_ista const& model_params,
	size_t const hw_neuron_id,
	NeuronCalibrationParameters const& params) const
{
	return _applyNeuronCalibration(model_params, hw_neuron_id, params);
}

HWNeuronParameter NeuronCollection::applyNeuronCalibration(
	PyNNParameters::IF_cond_exp const& model_params,
	size_t const hw_neuron_id,
	NeuronCalibrationParameters const& params) const
{
	return _applyNeuronCalibration(model_params, hw_neuron_id, params);
}

size_t NeuronCollection::getSpeedup() const
{
	return mSpeedup;
}

void NeuronCollection::setSpeedup(size_t const s)
{
	mSpeedup = s;
}

size_t NeuronCollection::getClock() const
{
	return mPLLFrequency / 4;
}

size_t NeuronCollection::getPLLFrequency() const
{
	return mPLLFrequency;
}

void NeuronCollection::setPLLFrequency(size_t const freq)
{
	mPLLFrequency = freq;
}

size_t NeuronCollection::getStartingCycle() const
{
	return mStartingCycle;
}

void NeuronCollection::setStartingCycle(size_t const sc)
{
	mStartingCycle = sc;
}

boost::shared_ptr<NeuronCollection> NeuronCollection::create()
{
	return boost::shared_ptr<NeuronCollection>(
		new NeuronCollection());
}

void NeuronCollection::copy(Collection const& rhs)
{
	*this = dynamic_cast<NeuronCollection const&>(rhs);
}

std::ostream& NeuronCollection::operator<< (std::ostream& os) const
{
	os << "NeuronCollection:"
		<< " speedup: " << mSpeedup
		<< " pll: " << mPLLFrequency
		<< "\n";
	return Collection::operator<<(os);
}

} // HMF

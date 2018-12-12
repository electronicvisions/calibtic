#pragma once
#include <stdexcept>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <iostream>

#include "calibtic/Calibration.h"
#include "hal/HICANNContainer.h"

namespace HMF {

// holds transformation for all synapses of a synapse row
// typically one polynomial that transforms a digital into an analog weight.
// Note that this is different to the trafo of neuron parameters, where the 
// polynomial represents the trafo from model to hardware parameters.
// offers methods to give the minimum and maximum analog weight,
// corresponding to digital weight 0, and 15 respectively
class SynapseCalibration :
	public calibtic::Calibration
{
public:
	typedef double analog_weight_t;

	SynapseCalibration();
	virtual ~SynapseCalibration();

	void setDefaults();

	/// return the digital_weight, that best implements the requested analog weight.
	/// the result is clipped to the HW range (0..15)
	/// stochastic rounding is applied in order to avoid systematic strengthening or weaking due to rounding.
	HICANN::SynapseWeight getDigitalWeight(
		double const analog_weight //<! analog weight in nano Siemens.
		) const;

	/// returns analog weight for a given digital weight in nano Siemens.
	analog_weight_t getAnalogWeight(HICANN::SynapseWeight const digital_weight) const;

	/// returns minimum analog weight in nano Siemens.
	analog_weight_t getMinAnalogWeight() const;

	/// returns maximum analog weight in nano Siemens.
	analog_weight_t getMaxAnalogWeight() const;

	virtual std::ostream& operator<< (std::ostream&) const;

	// Py++ factory function
	static
	boost::shared_ptr<SynapseCalibration> create();

	virtual void copy(Calibration const&);

	/// checks whether analog weights are monotonic increasing for increasing digital weights.
	bool check_monotonic_increasing() const;
private:
	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};
} // HMF



// implementation
namespace HMF {

template<typename Archiver>
void SynapseCalibration::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Calibration);
}

} // HMF

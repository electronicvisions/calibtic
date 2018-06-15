#pragma once
#include <stdexcept>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <iostream>

#include "calibtic/Calibration.h"

namespace HMF {

// holds transformation of the utilization of synaptic efficacy in STP mechnamism.
// typically one polynomial that transforms the cap (0..7) setting into parameter U_{SE}
// Note that this is different to the trafo of neuron parameters, where the 
// polynomial represents the trafo from model to hardware parameters.
class STPUtilizationCalibration :
	public calibtic::Calibration
{
public:
	typedef double utilization_t;
	static const uint8_t minCap = 0;
	static const uint8_t maxCap = 7;

	STPUtilizationCalibration();
	virtual ~STPUtilizationCalibration();

	void setDefaults();

	/// return the cap configuration, that best implements the requested U_SE value.
	/// the result is clipped to the HW range (0..7)
	/// stochastic rounding is applied in order to avoid systematic strengthening or weaking due to rounding.
	uint8_t getCap(
		double const utilization //<! utilization parameter U_SE in hardware STP model
		) const;

	/// returns utilization U_SE for a cap setting 
	utilization_t getUtilization(uint8_t const cap) const;

	/// returns minimum utilization U_SE
	utilization_t getMinUtilization() const;

	/// returns maximum utilization U_SE
	utilization_t getMaxUtilization() const;

	virtual std::ostream& operator<< (std::ostream&) const;

	// Py++ factory function
	static
	boost::shared_ptr<STPUtilizationCalibration> create();

	virtual void copy(Calibration const&);

	/// checks whether utilization valus are monotonic increasing for increasing cap configs.
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
void STPUtilizationCalibration::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Calibration);
}

} // HMF

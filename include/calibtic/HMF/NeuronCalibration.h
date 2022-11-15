#pragma once
#include <stdexcept>

#include <boost/operators.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <iostream>

#include "calibtic/Calibration.h"
#include "calibtic/HMF/HWNeuronParameter.h"
#include "euter/cellparameters.h"
// GCCXML doesn't like the new log4cxx headers, avoid including them
#ifdef PYPLUSPLUS
#include <boost/shared_ptr.hpp>
namespace log4cxx {
class Logger;
typedef boost::shared_ptr<log4cxx::Logger> LoggerPtr;
}
#else
#include <log4cxx/logger.h>
#endif // PYPLUSPLUS
#include <vector>

namespace HMF {

struct NeuronCalibrationParameters
	: public boost::operators<NeuronCalibrationParameters>
{
	PYPP_INIT(size_t hw_neuron_size, 1);
	PYPP_INIT(bool bigcap, true);
	PYPP_INIT(bool I_gl_slow, false);
	PYPP_INIT(bool I_gl_fast, false);
	PYPP_INIT(bool I_gladapt_slow, false);
	PYPP_INIT(bool I_gladapt_fast, false);
	PYPP_INIT(bool I_radapt_slow, false);
	PYPP_INIT(bool I_radapt_fast, false);
	PYPP_INIT(double alphaV, 10); // unitless scaling factor
	PYPP_INIT(double shiftV, 1.2); // shift in Volt

	NeuronCalibrationParameters();

	// calibrations including bigcap and speedup combinations
	struct Calibrations {
		enum calib {
			E_l,
			E_syni,
			E_synx,
			I_bexp,
			I_convi,
			I_convx,
			I_fire,
			I_gl_slow0_fast0_bigcap0, // 0
			I_gl_slow0_fast1_bigcap0, // 1
			I_gl_slow1_fast0_bigcap0, // 2
			I_gl_slow1_fast1_bigcap0, // 3
			I_gl_slow0_fast0_bigcap1, // 4
			I_gl_slow0_fast1_bigcap1, // 5
			I_gl_slow1_fast0_bigcap1, // 6
			I_gl_slow1_fast1_bigcap1, // 7
			I_gladapt_slow0_fast0_bigcap0, // 0
			I_gladapt_slow0_fast1_bigcap0, // 1
			I_gladapt_slow1_fast0_bigcap0, // 2
			I_gladapt_slow1_fast1_bigcap0, // 3
			I_gladapt_slow0_fast0_bigcap1, // 4
			I_gladapt_slow0_fast1_bigcap1, // 5
			I_gladapt_slow1_fast0_bigcap1, // 6
			I_gladapt_slow1_fast1_bigcap1, // 7
			I_intbbi,
			I_intbbx,
			I_pl,
			I_radapt_slow0_fast0_bigcap0, // 0
			I_radapt_slow0_fast1_bigcap0, // 1
			I_radapt_slow1_fast0_bigcap0, // 2
			I_radapt_slow1_fast1_bigcap0, // 3
			I_radapt_slow0_fast0_bigcap1, // 4
			I_radapt_slow0_fast1_bigcap1, // 5
			I_radapt_slow1_fast0_bigcap1, // 6
			I_radapt_slow1_fast1_bigcap1, // 7
			I_rexp,
			I_spikeamp,
			V_exp,
			V_syni,
			V_syntci,
			V_syntcx,
			V_synx,
			V_t,
			V_convoffi,
			V_convoffx,
			ReadoutShift,
			BigcapToSmallcap, // ratio of big to small capacitance
			NCAL_SIZE
		};
	};

	// mapping of calibrations to hardware parameters
	std::vector<HICANN::neuron_parameter> Calibrations_to_HW;

	// returns the correct I_gl enum for the current bigcap/speedup combination
	Calibrations::calib I_gl() const {
		return Calibrations::calib(bigcap * 4 + I_gl_slow * 2 + I_gl_fast * 1 +
								 Calibrations::I_gl_slow0_fast0_bigcap0);
	};

	// returns the I_gladapt enum for the current speedup combination
	Calibrations::calib I_gladapt() const {
		return Calibrations::calib(bigcap * 4 + I_gladapt_slow * 2 + I_gladapt_fast * 1 +
		                           Calibrations::I_gladapt_slow0_fast0_bigcap0);
	};

	// returns the I_radapt enum for the current speedup combination
	Calibrations::calib I_radapt() const {
		return Calibrations::calib(bigcap * 4 + I_radapt_slow * 2 + I_radapt_fast * 1 +
		                           Calibrations::I_radapt_slow0_fast0_bigcap0);
	};

	// if the bigcap switch is on, the total capacitance is 2.16456 pF ("fast mode")
	// if the switch is off, the capacitance is 0.1642 pF ("slow mode")
	double cap() const {
		return (bigcap ? big_cap : small_cap);
	};

    // hardware capacitances
    static const double big_cap;        //technical value of the big capacitance
    static const double small_cap;      //technical value of the small capacitance

	// false if different from default
	bool modified() const;
	bool operator==(NeuronCalibrationParameters rhs) const;
};

std::string to_string(HMF::NeuronCalibrationParameters::Calibrations::calib c);

class NeuronCalibration :
	public calibtic::Calibration
{
public:

	typedef NeuronCalibrationParameters::Calibrations Calibrations;

	NeuronCalibration(bool has_default=true);
	virtual ~NeuronCalibration();

	void setDefaults();
	void setHICANNv4Defaults();

	HWNeuronParameter applyNeuronCalibration(
		PyNNParameters::EIF_cond_exp_isfa_ista const& p,
		double const speedup,
		NeuronCalibrationParameters const& = NeuronCalibrationParameters()) const;

	HWNeuronParameter applyNeuronCalibration(
		PyNNParameters::IF_cond_exp const& p,
		double const speedup,
		NeuronCalibrationParameters const& = NeuronCalibrationParameters()) const;

	PyNNParameters::EIF_cond_exp_isfa_ista applyNeuronReverse(
		HWNeuronParameter const& h,
		double const speedup, double const cm_bio,
		NeuronCalibrationParameters const& = NeuronCalibrationParameters()) const;

    //scales biological parameters to the technical regime of the hardware, no calibration!
    PyNNParameters::EIF_cond_exp_isfa_ista scaleParameters(
		PyNNParameters::EIF_cond_exp_isfa_ista const& p,
		double const shiftV, double const alphaV,
		double const speedup, double const cap) const;

	virtual std::ostream& operator<< (std::ostream&) const;

	void check() const;

	// Py++ factory function
	static
	boost::shared_ptr<NeuronCalibration> create();

	virtual void copy(Calibration const&);

	//scaling BioParameter

	/// scales a PyNN voltage parameter to the hardware domain.
	/// performs: returnval = val*1e-3*mAlphaV + mShiftV
	/// where mAlphaV and mShiftV are the voltage scaling and shift parameters
	/// and 1.e-3 represents the mV to Volt trafo.
	///
	/// @param val bio voltage in mV
	/// @returns hardware voltage in Volt
	/// @note don't use this for PyNN AdEx Parameter delta_T
	double scaleVoltage(double val, double shiftV, double alphaV) const; //!< mV (bio) -> V (hardware)
	double scaleCurrent(double val, double speedup, double cm, double alphaV, double cap) const; //!< nA (bio) -> A (hardware)
	double scaleVoltageDeltaT(double val, double alphaV) const; //!< mV (bio) -> V (hardware)
	double scaleConductance(double val, double speedup, double cm, double cap) const; //!< nS (bio) -> S (hardware)
	double scaleTau(double val, double speedup) const; //!< ms (bio) -> s (hardware)

	// Reverse scaling from hardware domain to biological domain
	double reverseScaleVoltage(double val, double shiftV, double alphaV) const; //!< V (hardware) -> mV (bio)
	double reverseScaleCurrent(double val, double speedup, double cm, double alphaV, double cap) const; //!< A (hardware) -> nA (bio)
	double reverseScaleVoltageDeltaT(double val, double alphaV) const; //!< V (hardware) -> mV (bio)
	double reverseScaleConductance(double val, double speedup, double cm, double cap) const; //!< S (hardware) -> nS (bio)
	double reverseScaleTau(double val, double speedup) const; //!< s (hardware) -> ms (bio)

	// clips fg_value to min/max_fg_value
	static int clip_fg_value(int fg_value);

	// technical analog -> digital DAC values
	// parameter v in to_dac() must be in SI units, e.g. Ampere, Volt, Second, ...
	int to_dac(double const v, Calibrations::calib const p,
	           trafo_t::OutsideDomainBehavior outside_domain_behavior =
	               trafo_t::CLIP) const; //!< SI unit -> DAC

	// digital DAC values -> analog
	// parameter v must be in DAC, result will be in SI units e.g. Ampere, Volt, Second, ...
	double from_dac(int const v, Calibrations::calib const p,
	                trafo_t::OutsideDomainBehavior outside_domain_behavior =
	                    trafo_t::CLIP) const; //!< DAC -> SI unit

	// transforms ideally from Volt to DAC, i.e.: DAC = v/max_techn_volt*max_fg_value
	// parameter v must be in Volt
	// NO CALIBRATION WILL BE APPLIED!
	static int ideal_volt_to_dac(double const v);

    // hardware capacitances
    static const double big_cap;        //technical value of the big capacitance
    static const double small_cap;      //technical value of the small capacitance

private:
	typedef HWNeuronParameter::value_type hw_value;

	void setExponentialTerm(
		PyNNParameters::EIF_cond_exp_isfa_ista const& p,
		std::vector<hw_value>& h,
		double const shiftV, double const alphaV) const;

	void setExponentialTerm(
		PyNNParameters::IF_cond_exp const& p,
		std::vector<hw_value>& h,
		double const shiftV, double const alphaV) const;

	/// disable the exponential spike generation
	void disableExponentialTerm( std::vector<hw_value>& h ) const;

	void setAdaptionParameters(
		PyNNParameters::EIF_cond_exp_isfa_ista const& p,
		std::vector<hw_value>& h,
		double const speedup,
		NeuronCalibrationParameters const&) const;

	void setAdaptionParameters(
		PyNNParameters::IF_cond_exp const& p,
		std::vector<hw_value>& h,
		double const speedup,
		NeuronCalibrationParameters const&) const;

	/// disable the spike triggered adaptation
	/// handles the case b = 0.
	void disableSpikeTriggeredAdaptation( std::vector<hw_value>& h ) const;

	/// disable the sub-threshold adaptation
	/// handles the case a = 0.
	void disableSubThresholdAdaptation( std::vector<hw_value>& h ) const;

	void setSpikingThreshold(
		PyNNParameters::EIF_cond_exp_isfa_ista const& p,
		std::vector<hw_value>& h,
		double const shiftV, double const alphaV) const;

	void setSpikingThreshold(
		PyNNParameters::IF_cond_exp const& p,
		std::vector<hw_value>& h,
		double const shiftV, double const alphaV) const;

	template<typename CellType>
	HWNeuronParameter _applyNeuronCalibration(
		CellType const& p,
		double const speedup,
		NeuronCalibrationParameters const&) const;

	void populateWithDefault(NeuronCalibration* cal) const;

#ifndef PYPLUSPLUS
	NeuronCalibration* mDefault;
#endif

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);

public:
	static const double max_techn_volt; //maximal technical voltage in V = 1.8
	static const double max_techn_curr; //maximal technical current in nA = 2500
	static const int    max_fg_value;   //maximal value of the fg-cell  = 1023
    static const int    min_fg_value;   //mininmal value of the fg-cell = 0

private:
	static const double mV_to_V; ///< multiplicative factor to scale voltages from mV to Volt
	static const double ms_to_s; ///< multiplicative factor to scale time constants from ms to s
	static const double nA_to_A; ///< multiplicative factor to scale currents from nA to A
	static const double nS_to_S; ///< multiplicative factor to scale conductance from nS to S
};

} // HMF




// implementation
namespace HMF {

template<typename Archiver>
void NeuronCalibration::serialize(Archiver& ar, unsigned int const version)
{
#ifndef PYPLUSPLUS
	using namespace boost::serialization;

	switch (version) {
		case 0:
		case 1:
		case 2: {
			LOG4CXX_ERROR(::log4cxx::Logger::getLogger("Calibtic"),
			              "Backwards serialization compatibility dropped for "
			              "NeuronCalibration versions < 3");
			throw std::runtime_error("Backwards serialization compatibility dropped for "
			                         "NeuronCalibration versions < 3");
			break;
		}
		case 3: {
			ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Calibration);
			break;
		}
		default: {
			throw std::runtime_error(
			    "calibtic::NeuronCalibration unknown serialization version");
		}
	}
#endif // !PYPLUSPLUS
}

} // HMF

BOOST_CLASS_VERSION(HMF::NeuronCalibration, 3)

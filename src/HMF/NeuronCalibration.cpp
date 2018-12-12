#include "calibtic/HMF/NeuronCalibration.h"

#include <string>
#include <valarray>

#include "calibtic/trafo/Polynomial.h"
#include "calibtic/trafo/NegativePowersPolynomial.h"
#include "calibtic/trafo/OneOverPolynomial.h"
#include "calibtic/trafo/Constant.h"
#include "calibtic/trafo/Lookup.h"

#include <type_traits>

using namespace std;
using namespace PyNNParameters;

static log4cxx::LoggerPtr _log = log4cxx::Logger::getLogger("Calibtic");

namespace {
calibtic::trafo::Lookup::data_type v_syntcx_lookup = {
#include "calibtic/data/HMF/v_syntcx_lookup.dat"
};
calibtic::trafo::Lookup::data_type v_syntci_lookup = {
#include "calibtic/data/HMF/v_syntci_lookup.dat"
};
calibtic::trafo::Lookup::data_type i_pl_lookup = {
#include "calibtic/data/HMF/i_pl_lookup.dat"
};
calibtic::trafo::Lookup::data_type i_gl_slow0_fast0_bigcap1_fit_parameters = {
#include "calibtic/data/HMF/i_gl_slow0_fast0_bigcap1_fit_parameters.dat"
};
}

namespace HMF {

std::string to_string(HMF::NeuronCalibrationParameters::Calibrations::calib c)
{
	using HNC = HMF::NeuronCalibrationParameters::Calibrations;
	switch (c) {
		case HNC::E_l:
			return "E_l";
		case HNC::E_syni:
			return "E_syni";
		case HNC::E_synx:
			return "E_synx";
		case HNC::I_bexp:
			return "I_bexp";
		case HNC::I_convi:
			return "I_convi";
		case HNC::I_convx:
			return "I_convx";
		case HNC::I_fire:
			return "I_fire";
		case HNC::I_gl_slow0_fast0_bigcap0:
			return "I_gl_slow0_fast0_bigcap0";
		case HNC::I_gl_slow0_fast1_bigcap0:
			return "I_gl_slow0_fast1_bigcap0";
		case HNC::I_gl_slow1_fast0_bigcap0:
			return "I_gl_slow1_fast0_bigcap0";
		case HNC::I_gl_slow1_fast1_bigcap0:
			return "I_gl_slow1_fast1_bigcap0";
		case HNC::I_gl_slow0_fast0_bigcap1:
			return "I_gl_slow0_fast0_bigcap1";
		case HNC::I_gl_slow0_fast1_bigcap1:
			return "I_gl_slow0_fast1_bigcap1";
		case HNC::I_gl_slow1_fast0_bigcap1:
			return "I_gl_slow1_fast0_bigcap1";
		case HNC::I_gl_slow1_fast1_bigcap1:
			return "I_gl_slow1_fast1_bigcap1";
		case HNC::I_gladapt_slow0_fast0_bigcap0:
			return "I_gladapt_slow0_fast0_bigcap0";
		case HNC::I_gladapt_slow0_fast1_bigcap0:
			return "I_gladapt_slow0_fast1_bigcap0";
		case HNC::I_gladapt_slow1_fast0_bigcap0:
			return "I_gladapt_slow1_fast0_bigcap0";
		case HNC::I_gladapt_slow1_fast1_bigcap0:
			return "I_gladapt_slow1_fast1_bigcap0";
		case HNC::I_gladapt_slow0_fast0_bigcap1:
			return "I_gladapt_slow0_fast0_bigcap1";
		case HNC::I_gladapt_slow0_fast1_bigcap1:
			return "I_gladapt_slow0_fast1_bigcap1";
		case HNC::I_gladapt_slow1_fast0_bigcap1:
			return "I_gladapt_slow1_fast0_bigcap1";
		case HNC::I_gladapt_slow1_fast1_bigcap1:
			return "I_gladapt_slow1_fast1_bigcap1";
		case HNC::I_intbbi:
			return "I_intbbi";
		case HNC::I_intbbx:
			return "I_intbbx";
		case HNC::I_pl:
			return "I_pl";
		case HNC::I_radapt_slow0_fast0_bigcap0:
			return "I_radapt_slow0_fast0_bigcap0";
		case HNC::I_radapt_slow0_fast1_bigcap0:
			return "I_radapt_slow0_fast1_bigcap0";
		case HNC::I_radapt_slow1_fast0_bigcap0:
			return "I_radapt_slow1_fast0_bigcap0";
		case HNC::I_radapt_slow1_fast1_bigcap0:
			return "I_radapt_slow1_fast1_bigcap0";
		case HNC::I_radapt_slow0_fast0_bigcap1:
			return "I_radapt_slow0_fast0_bigcap1";
		case HNC::I_radapt_slow0_fast1_bigcap1:
			return "I_radapt_slow0_fast1_bigcap1";
		case HNC::I_radapt_slow1_fast0_bigcap1:
			return "I_radapt_slow1_fast0_bigcap1";
		case HNC::I_radapt_slow1_fast1_bigcap1:
			return "I_radapt_slow1_fast1_bigcap1";
		case HNC::I_rexp:
			return "I_rexp";
		case HNC::I_spikeamp:
			return "I_spikeamp";
		case HNC::V_exp:
			return "V_exp";
		case HNC::V_syni:
			return "V_syni";
		case HNC::V_syntci:
			return "V_syntci";
		case HNC::V_syntcx:
			return "V_syntcx";
		case HNC::V_synx:
			return "V_synx";
		case HNC::V_t:
			return "V_t";
		case HNC::V_convoffi:
			return "V_convoffi";
		case HNC::V_convoffx:
			return "V_convoffx";
		case HNC::ReadoutShift:
			return "ReadoutShift";
		case HNC::BigcapToSmallcap:
			return "BigcapToSmallcap";
		case HNC::NCAL_SIZE:
			return "NCAL_SIZE";
		default: {
			throw std::out_of_range(
			    "to_string: not a HMF::NeuronCalibrationParameters::Calibrations " +
			    std::to_string(static_cast<int>(c)));
		}
	};
}

const double NeuronCalibrationParameters::big_cap   = 2.16456e-3;  //nF
const double NeuronCalibrationParameters::small_cap =   164.2e-6;  //nF

const double NeuronCalibration::big_cap   = NeuronCalibrationParameters::big_cap;
const double NeuronCalibration::small_cap = NeuronCalibrationParameters::small_cap;

const double NeuronCalibration::max_techn_volt =        1.8;  //V
const double NeuronCalibration::max_techn_curr =      2500.;  //nA
const int    NeuronCalibration::max_fg_value   =       1023;
const int    NeuronCalibration::min_fg_value   =          0;
const double NeuronCalibration::mV_to_V        =      0.001; // multiplicative factor to scale voltages from mV to Volt
const double NeuronCalibration::ms_to_s        =      0.001; // multiplicative factor to scale time constants from ms to s
const double NeuronCalibration::nA_to_A        =       1e-9; // multiplicative factor to scale currents from nA to A
const double NeuronCalibration::nS_to_S        =       1e-9; // multiplicative factor to scale conductances from nS to S

NeuronCalibrationParameters::NeuronCalibrationParameters()
    : Calibrations_to_HW(Calibrations::NCAL_SIZE) {
	Calibrations_to_HW[Calibrations::E_l] = HICANN::E_l;
	Calibrations_to_HW[Calibrations::E_syni] = HICANN::E_syni;
	Calibrations_to_HW[Calibrations::E_synx] = HICANN::E_synx;
	Calibrations_to_HW[Calibrations::I_bexp] = HICANN::I_bexp;
	Calibrations_to_HW[Calibrations::I_convi] = HICANN::I_convi;
	Calibrations_to_HW[Calibrations::I_convx] = HICANN::I_convx;
	Calibrations_to_HW[Calibrations::I_fire] = HICANN::I_fire;
	Calibrations_to_HW[Calibrations::I_gl_slow0_fast0_bigcap0] = HICANN::I_gl;
	Calibrations_to_HW[Calibrations::I_gl_slow0_fast1_bigcap0] = HICANN::I_gl;
	Calibrations_to_HW[Calibrations::I_gl_slow1_fast0_bigcap0] = HICANN::I_gl;
	Calibrations_to_HW[Calibrations::I_gl_slow1_fast1_bigcap0] = HICANN::I_gl;
	Calibrations_to_HW[Calibrations::I_gl_slow0_fast0_bigcap1] = HICANN::I_gl;
	Calibrations_to_HW[Calibrations::I_gl_slow0_fast1_bigcap1] = HICANN::I_gl;
	Calibrations_to_HW[Calibrations::I_gl_slow1_fast0_bigcap1] = HICANN::I_gl;
	Calibrations_to_HW[Calibrations::I_gl_slow1_fast1_bigcap1] = HICANN::I_gl;
	Calibrations_to_HW[Calibrations::I_gladapt_slow0_fast0_bigcap0] = HICANN::I_gladapt;
	Calibrations_to_HW[Calibrations::I_gladapt_slow0_fast1_bigcap0] = HICANN::I_gladapt;
	Calibrations_to_HW[Calibrations::I_gladapt_slow1_fast0_bigcap0] = HICANN::I_gladapt;
	Calibrations_to_HW[Calibrations::I_gladapt_slow1_fast1_bigcap0] = HICANN::I_gladapt;
	Calibrations_to_HW[Calibrations::I_gladapt_slow0_fast0_bigcap1] = HICANN::I_gladapt;
	Calibrations_to_HW[Calibrations::I_gladapt_slow0_fast1_bigcap1] = HICANN::I_gladapt;
	Calibrations_to_HW[Calibrations::I_gladapt_slow1_fast0_bigcap1] = HICANN::I_gladapt;
	Calibrations_to_HW[Calibrations::I_gladapt_slow1_fast1_bigcap1] = HICANN::I_gladapt;
	Calibrations_to_HW[Calibrations::I_intbbi] = HICANN::I_intbbi;
	Calibrations_to_HW[Calibrations::I_intbbx] = HICANN::I_intbbx;
	Calibrations_to_HW[Calibrations::I_pl] = HICANN::I_pl;
	Calibrations_to_HW[Calibrations::I_radapt_slow0_fast0_bigcap0] = HICANN::I_radapt;
	Calibrations_to_HW[Calibrations::I_radapt_slow0_fast1_bigcap0] = HICANN::I_radapt;
	Calibrations_to_HW[Calibrations::I_radapt_slow1_fast0_bigcap0] = HICANN::I_radapt;
	Calibrations_to_HW[Calibrations::I_radapt_slow1_fast1_bigcap0] = HICANN::I_radapt;
	Calibrations_to_HW[Calibrations::I_radapt_slow0_fast0_bigcap1] = HICANN::I_radapt;
	Calibrations_to_HW[Calibrations::I_radapt_slow0_fast1_bigcap1] = HICANN::I_radapt;
	Calibrations_to_HW[Calibrations::I_radapt_slow1_fast0_bigcap1] = HICANN::I_radapt;
	Calibrations_to_HW[Calibrations::I_radapt_slow1_fast1_bigcap1] = HICANN::I_radapt;
	Calibrations_to_HW[Calibrations::I_rexp] = HICANN::I_rexp;
	Calibrations_to_HW[Calibrations::I_spikeamp] = HICANN::I_spikeamp;
	Calibrations_to_HW[Calibrations::V_exp] = HICANN::V_exp;
	Calibrations_to_HW[Calibrations::V_syni] = HICANN::V_syni;
	Calibrations_to_HW[Calibrations::V_syntci] = HICANN::V_syntci;
	Calibrations_to_HW[Calibrations::V_syntcx] = HICANN::V_syntcx;
	Calibrations_to_HW[Calibrations::V_synx] = HICANN::V_synx;
	Calibrations_to_HW[Calibrations::V_t] = HICANN::V_t;
	Calibrations_to_HW[Calibrations::V_convoffi] = HICANN::V_convoffi;
	Calibrations_to_HW[Calibrations::V_convoffx] = HICANN::V_convoffx;
}

bool NeuronCalibrationParameters::modified() const
{
	return *this != NeuronCalibrationParameters{};
}

bool NeuronCalibrationParameters::operator==(NeuronCalibrationParameters rhs) const
{
	return (hw_neuron_size == rhs.hw_neuron_size &&
	        bigcap == rhs.bigcap &&
	        I_gl_slow == rhs.I_gl_slow &&
	        I_gl_fast == rhs.I_gl_fast &&
	        I_gladapt_slow == rhs.I_gladapt_slow &&
	        I_gladapt_fast == rhs.I_gladapt_fast &&
	        I_radapt_slow == rhs.I_radapt_slow &&
	        I_radapt_fast == rhs.I_radapt_fast &&
	        alphaV == rhs.alphaV &&
	        shiftV == rhs.shiftV);
}

void NeuronCalibration::populateWithDefault(NeuronCalibration* cal) const
{

	using namespace calibtic::trafo;
	using NPP = NegativePowersPolynomial;

	// I_gl --------------------------------------------------------------------
	//
	// Scaling factors of the current mirrors:
	//
	// slow fast divisor    sthal
	// 0    0    3       <- normal
	// 0    1    1       <- fast
	// 1    0    27      <- slow

	size_t const I_gl_divisor_slow1_fast0 = 27;
	size_t const I_gl_divisor_slow0_fast0 = 3;
	size_t const I_gl_divisor_slow0_fast1 = 1;

	double const fast_to_normal =
	    static_cast<double>(I_gl_divisor_slow0_fast1) / I_gl_divisor_slow0_fast0;
	double const slow_to_normal =
	    static_cast<double>(I_gl_divisor_slow1_fast0) / I_gl_divisor_slow0_fast0;

	// experimentally determined ratio
	double const big_to_smallcap_tau_m_factor = 1 / 5.;

	// factors for a fit to bigcap with normal speedup
	std::valarray<double> const npps(
	    i_gl_slow0_fast0_bigcap1_fit_parameters.data(),
	    i_gl_slow0_fast0_bigcap1_fit_parameters.size());
	std::valarray<double> const big_to_smallcap_tau_m_factors = {
	    pow(big_to_smallcap_tau_m_factor, 0), pow(big_to_smallcap_tau_m_factor, 1),
	    pow(big_to_smallcap_tau_m_factor, 2), pow(big_to_smallcap_tau_m_factor, 3),
	    pow(big_to_smallcap_tau_m_factor, 4)};

	// smallcap
	// -> slow
	std::valarray<double> const smallcap_slow =
	    npps * big_to_smallcap_tau_m_factors * slow_to_normal;
	cal->reset(
	    Calibrations::I_gl_slow1_fast0_bigcap0,
	    NPP::create({std::begin(smallcap_slow), std::end(smallcap_slow)}, 3.00e-7, 7.22e-5));
	// -> normal
	std::valarray<double> const smallcap_normal = npps * big_to_smallcap_tau_m_factors;
	cal->reset(
	    Calibrations::I_gl_slow0_fast0_bigcap0,
	    NPP::create({std::begin(smallcap_normal), std::end(smallcap_normal)}, 1.083e-7, 9.63e-6));
	// -> fast
	std::valarray<double> const smallcap_fast =
	    npps * big_to_smallcap_tau_m_factors * fast_to_normal;
	cal->reset(
	    Calibrations::I_gl_slow0_fast1_bigcap0,
	    NPP::create({std::begin(smallcap_fast), std::end(smallcap_fast)}, 1.00e-7, 4.02e-6));

	// bigcap
	// -> slow
	std::valarray<double> const bigcap_slow = npps * slow_to_normal;
	cal->reset(
	    Calibrations::I_gl_slow1_fast0_bigcap1,
	    NPP::create({std::begin(bigcap_slow), std::end(bigcap_slow)}, 1.50e-6, 2.00e-4));
	// -> normal
	std::valarray<double> const bigcap_normal = npps;
	cal->reset(
	    Calibrations::I_gl_slow0_fast0_bigcap1,
	    NPP::create({std::begin(npps), std::end(npps)}, 5.41e-7, 4.82e-5));
	// -> fast
	std::valarray<double> const bigcap_fast = npps * fast_to_normal;
	cal->reset(
	    Calibrations::I_gl_slow0_fast1_bigcap1,
	    NPP::create({std::begin(bigcap_fast), std::end(bigcap_fast)}, 4.23e-7, 2.01e-5));
	// -------------------------------------------------------------------------

	// leak a.k.a. rest potential
	cal->reset(Calibrations::E_l,       Polynomial::create({0.0, 1023./1.8},               0.0,    1.8)); // v_rest, V -> DAC

	// excitatory & inhibitory reversal potential
	cal->reset(Calibrations::E_synx,    Polynomial::create({-16, 614},                     0.0,    1.8)); // e_rev_E, V -> DAC
	cal->reset(Calibrations::E_syni,    Polynomial::create({0.0, 1023./1.8},               0.0,    1.8)); // e_rev_I, V -> DAC

	// adaption variables
	//
	// FIXME: the same default transformation is used for all speedup and bigcap settings (#2298)!
	//
	// I_gladapt ---------------------------------------------------------------
	//
	// smallcap
	// -> normal
	cal->reset(Calibrations::I_gladapt_slow0_fast0_bigcap0,
	           Polynomial::create({-0.2701, 0.106392e9, 2.01736e13}, 2.537e-9,
	                              4958.e-9)); // a, S -> DAC
	// -> fast
	cal->reset(Calibrations::I_gladapt_slow0_fast1_bigcap0,
	           Polynomial::create({-0.2701, 0.106392e9, 2.01736e13}, 2.537e-9,
	                              4958.e-9)); // a, S -> DAC
	// -> slow
	cal->reset(Calibrations::I_gladapt_slow1_fast0_bigcap0,
	           Polynomial::create({-0.2701, 0.106392e9, 2.01736e13}, 2.537e-9,
	                              4958.e-9)); // a, S -> DAC

	// bigcap
	// -> normal
	cal->reset(Calibrations::I_gladapt_slow0_fast0_bigcap1,
	           Polynomial::create({-0.2701, 0.106392e9, 2.01736e13}, 2.537e-9,
	                              4958.e-9)); // a, S -> DAC
	// -> fast
	cal->reset(Calibrations::I_gladapt_slow0_fast1_bigcap1,
	           Polynomial::create({-0.2701, 0.106392e9, 2.01736e13}, 2.537e-9,
	                              4958.e-9)); // a, S -> DAC
	// -> slow
	cal->reset(Calibrations::I_gladapt_slow1_fast0_bigcap1,
	           Polynomial::create({-0.2701, 0.106392e9, 2.01736e13}, 2.537e-9,
	                              4958.e-9)); // a, S -> DAC
	// -------------------------------------------------------------------------

	// I_radapt ----------------------------------------------------------------
	//
	// FIXME: the same default transformation is used for all speedup and bigcap settings (#2298)!
	//
	// smallcap
	// -> normal
	cal->reset(Calibrations::I_radapt_slow0_fast0_bigcap0,
	           OneOverPolynomial::create({-1.222e-3, 7.82014e2, 1.07527e7}, 2.93e-6,
	                                     35.4e-6)); // tau_w, s -> DAC
	// -> fast
	cal->reset(Calibrations::I_radapt_slow0_fast1_bigcap0,
	           OneOverPolynomial::create({-1.222e-3, 7.82014e2, 1.07527e7}, 2.93e-6,
	                                     35.4e-6)); // tau_w, s -> DAC
	// -> slow
	cal->reset(Calibrations::I_radapt_slow1_fast0_bigcap0,
	           OneOverPolynomial::create({-1.222e-3, 7.82014e2, 1.07527e7}, 2.93e-6,
	                                     35.4e-6)); // tau_w, s -> DAC
	// -> slow and fast - not supported?
	cal->reset(Calibrations::I_radapt_slow1_fast1_bigcap0,
	           OneOverPolynomial::create({-1.222e-3, 7.82014e2, 1.07527e7}, 2.93e-6,
	                                     35.4e-6)); // tau_w, s -> DAC

	// bigcap
	// -> normal
	cal->reset(Calibrations::I_radapt_slow0_fast0_bigcap1,
	           OneOverPolynomial::create({-1.222e-3, 7.82014e2, 1.07527e7}, 2.93e-6,
	                                     35.4e-6)); // tau_w, s -> DAC
	// -> fast
	cal->reset(Calibrations::I_radapt_slow0_fast1_bigcap1,
	           OneOverPolynomial::create({-1.222e-3, 7.82014e2, 1.07527e7}, 2.93e-6,
	                                     35.4e-6)); // tau_w, s -> DAC
	// -> slow
	cal->reset(Calibrations::I_radapt_slow1_fast0_bigcap1,
	           OneOverPolynomial::create({-1.222e-3, 7.82014e2, 1.07527e7}, 2.93e-6,
	                                     35.4e-6)); // tau_w, s -> DAC
	// -> slow and fast - not supported?
	cal->reset(Calibrations::I_radapt_slow1_fast1_bigcap1,
	           OneOverPolynomial::create({-1.222e-3, 7.82014e2, 1.07527e7}, 2.93e-6,
	                                     35.4e-6)); // tau_w, s -> DAC
	// -------------------------------------------------------------------------

	// AdEx - exponential term
	cal->reset(Calibrations::V_exp,     Polynomial::create({0.0, 1023./1.8}, 0, 1.8)); // v_thresh, V -> DAC
	cal->reset(Calibrations::I_rexp,    Polynomial::create({-38.5688, 27.1646e3, 3.7804e6}, 1.21e-3,   13.5e-3)); // delta_T, V -> DAC
	cal->reset(Calibrations::V_t,       Polynomial::create({0.0, 1023./1.8},          0.0,    1.8)); // v_spike, V -> DAC

	cal->reset(Calibrations::I_fire,    Polynomial::create({22.4037, 18.414e9, -0.057288e18},         0.0324e-9, 69.26e-9)); // b, A -> DAC

	// refractory period
	cal->reset(Calibrations::I_pl, Lookup::create(i_pl_lookup, 10));
	// synaptic input
	cal->reset(Calibrations::V_syntcx,  Lookup::create(v_syntcx_lookup, 226)); // tau_syn_E, s -> DAC
	cal->reset(Calibrations::V_syntci,  Lookup::create(v_syntci_lookup, 226)); // tau_syn_I, s -> DAC

	// dark marco magic a.k.a. biases
	cal->reset(Calibrations::I_convi,   Constant::create(1023));
	cal->reset(Calibrations::I_convx,   Constant::create(1023));
	cal->reset(Calibrations::I_intbbi,  Constant::create(511));
	cal->reset(Calibrations::I_intbbx,  Constant::create(511));
	cal->reset(Calibrations::V_syni,    Constant::create(511));
	cal->reset(Calibrations::V_synx,    Constant::create(511));
	cal->reset(Calibrations::I_spikeamp,Constant::create(1023));
	cal->reset(Calibrations::I_bexp,    Constant::create(1023));

	// HICANNv4
	cal->reset(Calibrations::V_convoffi, Constant::create(1023));
	cal->reset(Calibrations::V_convoffx, Constant::create(1023));

	cal->reset(Calibrations::BigcapToSmallcap, Constant::create(1. / big_to_smallcap_tau_m_factor));
}

NeuronCalibration::NeuronCalibration(bool has_default) :
	Calibration(NeuronCalibrationParameters::Calibrations::NCAL_SIZE),
	mDefault(nullptr)
{
	if(has_default) {
		// default calibration does not hold default again
		mDefault = new NeuronCalibration(false);
		populateWithDefault(mDefault);
	}
}

NeuronCalibration::~NeuronCalibration()
{
	delete mDefault;
}

void NeuronCalibration::setDefaults()
{
	check();
	populateWithDefault(this);
}

void NeuronCalibration::setHICANNv4Defaults()
{
	using namespace calibtic::trafo;

	reset(Calibrations::V_convoffi, Constant::create(1023));
	reset(Calibrations::V_convoffx, Constant::create(1023));
}

ostream& NeuronCalibration::operator<< (ostream& os) const
{
	os << "NeuronCalibration: ";
	return Calibration::operator<<(os);
}

void NeuronCalibration::check() const
{
	if (size() != (unsigned int) NeuronCalibrationParameters::Calibrations::NCAL_SIZE) {
		throw runtime_error(
			std::string(__PRETTY_FUNCTION__) + ": wrong data set size in NeuronCalibration");
}
}

boost::shared_ptr<NeuronCalibration> NeuronCalibration::create()
{
	return boost::shared_ptr<NeuronCalibration>(new NeuronCalibration());
}

void NeuronCalibration::copy(Calibration const& rhs)
{
	*this = dynamic_cast<NeuronCalibration const&>(rhs);
}

HWNeuronParameter NeuronCalibration::applyNeuronCalibration(
	EIF_cond_exp_isfa_ista const& p,
	double const speedup,
	NeuronCalibrationParameters const& params) const
{
	return _applyNeuronCalibration(p, speedup, params);
}

HWNeuronParameter NeuronCalibration::applyNeuronCalibration(
	IF_cond_exp const& p,
	double const speedup,
	NeuronCalibrationParameters const& params) const
{
	return _applyNeuronCalibration(p, speedup, params);
}

PyNNParameters::EIF_cond_exp_isfa_ista NeuronCalibration::scaleParameters(
	PyNNParameters::EIF_cond_exp_isfa_ista const& p,
	double const shiftV, double const alphaV,
	double const speedup, double const cap) const
{
    PyNNParameters::EIF_cond_exp_isfa_ista scaled;
    //set Capacitance
    scaled.cm = cap;
    //scale LIF-params
    scaled.v_rest = scaleVoltage(p.v_rest, shiftV, alphaV);
    scaled.tau_m = scaleTau(p.tau_m, speedup);
    scaled.e_rev_E = scaleVoltage(p.e_rev_E, shiftV, alphaV);
    scaled.e_rev_I = scaleVoltage(p.e_rev_I, shiftV, alphaV);
    scaled.tau_refrac = scaleTau(p.tau_refrac, speedup);
    scaled.v_spike = scaleVoltage(p.v_spike, shiftV, alphaV);
    //scale exponential term
    scaled.v_thresh = scaleVoltage(p.v_thresh, shiftV, alphaV);
    scaled.delta_T = scaleVoltageDeltaT(p.delta_T, alphaV);
    //scale adaptation term
    scaled.a = scaleConductance(p.a, speedup, p.cm, cap);
    scaled.tau_w = scaleTau(p.tau_w, speedup);
    scaled.b = scaleCurrent(p.b, speedup, p.cm, alphaV, cap);
    //scale synaptic input
    scaled.tau_syn_E = scaleTau(p.tau_syn_E, speedup);
    scaled.tau_syn_I = scaleTau(p.tau_syn_I, speedup);

    return scaled;
}

//implemetation of scaling
double NeuronCalibration::scaleVoltage(double const val, double shiftV, double alphaV) const
{
    double returnval = val* mV_to_V * alphaV + shiftV;
	return returnval;
}

double NeuronCalibration::scaleCurrent(
	double const val, double const speedup, double const cm, double alphaV, double const cap) const
{
	return val * nA_to_A * alphaV * speedup * cap / cm;
}

double NeuronCalibration::scaleVoltageDeltaT(
	double const val, double const alphaV) const
{
	return val * alphaV * mV_to_V;
}

double NeuronCalibration::scaleConductance(
	double const val, double const speedup,
	double const cm, double const cap) const
{
	return val * nS_to_S * speedup * cap / cm;
}

double NeuronCalibration::scaleTau(
	double const val, double const speedup) const
{
	return val * ms_to_s / speedup;
}

//implemetation of reverse scaling
double NeuronCalibration::reverseScaleVoltage(double const val,
	double shiftV, double alphaV) const
{
	double returnval = (val - shiftV) / (mV_to_V * alphaV);
	return returnval;
}

double NeuronCalibration::reverseScaleCurrent(
	double const val, double const speedup, double const cm,
	double alphaV, double const cap) const
{
	return cm * val / (nA_to_A * alphaV * speedup * cap);
}

double NeuronCalibration::reverseScaleVoltageDeltaT(
	double const val, double const alphaV) const
{
	return val / alphaV / mV_to_V;
}

double NeuronCalibration::reverseScaleConductance(
	double const val, double const speedup,
	double const cm, double const cap) const
{
	return val * cm / (speedup * nS_to_S * cap);
}

double NeuronCalibration::reverseScaleTau(
	double const val, double const speedup) const
{
	return val / ms_to_s * speedup;
}

int NeuronCalibration::clip_fg_value(int fg_value)
{
	return std::max(min_fg_value, std::min(fg_value, max_fg_value));
}


int NeuronCalibration::to_dac(
    double const v, Calibrations::calib const p,
    trafo_t::OutsideDomainBehavior outside_domain_behavior) const {

	double val;

	try {
		applyOne(v, val, p, outside_domain_behavior);
	} catch (const std::exception& e) {
		LOG4CXX_INFO(
		    _log, "Calibtic::NeuronCalibration: "
		              << to_string(p) << " will be calibrated with a default transformation");
		if (mDefault) {
			mDefault->applyOne(v, val, p, outside_domain_behavior);
		} else {
			throw std::runtime_error("no default calibration available");
		}
	}

	const int dac = round(val);
	const int dac_clipped = clip_fg_value(dac);

	if (dac != dac_clipped)
	{
		LOG4CXX_WARN(
		    _log, "digital FG value " << dac << " of neuron parameter " << to_string(p)
		                              << " clipped to " << dac_clipped);
	}

	return dac_clipped;
}


double NeuronCalibration::from_dac(
    int const v, Calibrations::calib const p,
    trafo_t::OutsideDomainBehavior outside_domain_behavior) const {

	const int dac_clipped = clip_fg_value(v);

	double val;

	try {
		reverseApplyOne(dac_clipped, val, p, outside_domain_behavior);
	} catch (const std::exception& e) {
		LOG4CXX_INFO(
		    _log, "Calibtic::NeuronCalibration::from_dac "
		              << to_string(p) << " will be calibrated with a default transformation");
		if (mDefault) {
			mDefault->reverseApplyOne(dac_clipped, val, p, outside_domain_behavior);
		} else {
			throw std::runtime_error("Calibtic::NeuronCalibration::from_dac: no default "
			                         "calibration available");
		}
	}

	return val;
}

int NeuronCalibration::ideal_volt_to_dac(double const v)
{
	const int returnval = clip_fg_value(round(v/max_techn_volt*max_fg_value));
	return returnval;
}

void NeuronCalibration::setExponentialTerm(
	PyNNParameters::EIF_cond_exp_isfa_ista const& p,
	std::vector<hw_value>& h,
	double const shiftV, double const alphaV) const
{
    LOG4CXX_DEBUG(_log, "NeuronCalibration::setExponentialTerm");

	// delta_T == 0 disables exponential term
	if (p.delta_T != 0.) {
		//  v_thresh -> HICANN::neuron_parameter::V_exp
		try {
			h[HICANN::neuron_parameter::V_exp] = to_dac(scaleVoltage(p.v_thresh, shiftV, alphaV), Calibrations::V_exp);
		} catch (const std::exception& e) {
			h[HICANN::neuron_parameter::V_exp]  = 1023;
			LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate v_thresh, because: " << e.what());
		}
		LOG4CXX_DEBUG(_log, "v_thresh = " << p.v_thresh << " mV transformed to V_exp = " << h[HICANN::neuron_parameter::V_exp]);

		// delta_T -> HICANN::neuron_parameter::I_rexp
		try {
			h[HICANN::neuron_parameter::I_rexp] = to_dac(scaleVoltageDeltaT(p.delta_T, alphaV), Calibrations::I_rexp);
		} catch (const std::exception& e) {
			LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate delta_T, because: " << e.what());
			h[HICANN::neuron_parameter::I_rexp] = 1023;
		}
		LOG4CXX_DEBUG(_log, "delta_T = " << p.delta_T << " mV transformed to I_rexp = " << h[HICANN::neuron_parameter::I_rexp]);

		try {
			applyOne(-1 /* unused */, h[HICANN::neuron_parameter::I_bexp], Calibrations::I_bexp);
		} catch (const std::exception& e) {
			LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate V_thresh, because: " << e.what());
		}
		assert(isfinite(h[HICANN::neuron_parameter::I_bexp]));
		LOG4CXX_DEBUG(_log, "I_bexp set to " << h[HICANN::neuron_parameter::I_bexp]);
	}
	else {
		disableExponentialTerm(h);
	}
}

void NeuronCalibration::setExponentialTerm(
	PyNNParameters::IF_cond_exp const&,
	std::vector<hw_value>& h,
	double const /*shiftV*/, double const /*alphaV*/) const
{
	disableExponentialTerm(h);
}

void NeuronCalibration::disableExponentialTerm(
	std::vector<hw_value>& h) const
{
    LOG4CXX_DEBUG(_log, "NeuronCalibration::disableExponentialTerm");

	// There are several ways to disable the exponential term.
	// This has been investigated by David Stöckel:
	// http://www.kip.uni-heidelberg.de/cms/fileadmin/groups/vision/Downloads/Internship_Reports/report_stoeckel.pdf
	// We use one of these options, see Table 5 in the report.
	h[HICANN::neuron_parameter::V_exp]  = 1023;
    LOG4CXX_DEBUG(_log, "V_exp set to " << h[HICANN::neuron_parameter::V_exp]);
	h[HICANN::neuron_parameter::I_rexp] = 1023;
    LOG4CXX_DEBUG(_log, "I_rexp set to " << h[HICANN::neuron_parameter::I_rexp]);
	h[HICANN::neuron_parameter::I_bexp] = 1023;
    LOG4CXX_DEBUG(_log, "I_bexp set to " << h[HICANN::neuron_parameter::I_bexp]);
}

void NeuronCalibration::setAdaptionParameters(
	PyNNParameters::EIF_cond_exp_isfa_ista const& p,
	std::vector<hw_value>& h,
	double const speedup,
	NeuronCalibrationParameters const& params) const
{
    LOG4CXX_DEBUG(_log, "NeuronCalibration::setAdaptationParameters");

	// a -> HICANN::neuron_parameter::I_gladapt
	if (p.a != 0.) {
		try {
			h[HICANN::neuron_parameter::I_gladapt] = to_dac(scaleConductance(p.a, speedup, p.cm, params.cap()), params.I_gladapt());
		} catch (const std::exception& e) {
			LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate a, because: " << e.what());
		}
		LOG4CXX_DEBUG(_log, "a = " << p.a << " nS transformed to I_gladapt = " << h[HICANN::neuron_parameter::I_gladapt]);
	} else {
		disableSubThresholdAdaptation(h);
	}

	// b -> HICANN::neuron_parameter::I_fire
	if (p.b != 0.) {
		try {
			h[HICANN::neuron_parameter::I_fire] = to_dac(scaleCurrent(p.b, speedup, p.cm, params.alphaV, params.cap()), Calibrations::I_fire);
		} catch (const std::exception& e) {
			LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate b, because: " << e.what());
		}
		LOG4CXX_DEBUG(_log, "b = " << p.b << " nA transformed to I_fire = " << h[HICANN::neuron_parameter::I_fire]);
	} else {
		disableSpikeTriggeredAdaptation(h);
	}

	// tau_w -> HICANN::neuron_parameter::I_radapt
	try {
		h[HICANN::neuron_parameter::I_radapt] = to_dac(scaleTau(p.tau_w, speedup), params.I_radapt());
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate tau_w, because: " << e.what());
	}

    LOG4CXX_DEBUG(_log, "tau_w = " << p.tau_w << " ms transformed to I_radapt = " << h[HICANN::neuron_parameter::I_radapt]);

}

void NeuronCalibration::setAdaptionParameters(
	PyNNParameters::IF_cond_exp const&,
	std::vector<hw_value>& h,
	double const, NeuronCalibrationParameters const&) const
{
    LOG4CXX_DEBUG(_log, "NeuronCalibration::setAdaptationParameters_IF");

	disableSpikeTriggeredAdaptation(h);
	disableSubThresholdAdaptation(h);

	h[HICANN::neuron_parameter::I_radapt] = 1023;
    LOG4CXX_DEBUG(_log, "Setting I_radapt = " << h[HICANN::neuron_parameter::I_radapt]);
}

void NeuronCalibration::disableSpikeTriggeredAdaptation(
		std::vector<hw_value>& h ) const
{
	h[HICANN::neuron_parameter::I_fire] = 0;
    LOG4CXX_DEBUG(_log, "Setting I_fire = " << h[HICANN::neuron_parameter::I_fire]);
}

void NeuronCalibration::disableSubThresholdAdaptation(
		std::vector<hw_value>& h ) const
{
	h[HICANN::neuron_parameter::I_gladapt] = 0;
    LOG4CXX_DEBUG(_log, "Setting I_gladapt = " << h[HICANN::neuron_parameter::I_gladapt]);
}


void NeuronCalibration::setSpikingThreshold(
	PyNNParameters::EIF_cond_exp_isfa_ista const& p,
	std::vector<hw_value>& h,
	double const shiftV, double const alphaV) const
{
    LOG4CXX_DEBUG(_log, "NeuronCalibration::setSpikingThreshold");

	double effective_threshold = p.v_spike;

	// if delta_T == 0, the effective spike threshold in the model is the
	// minimum of v_spike and v_thresh, cf. #1776
	if ( p.delta_T == 0. && p.v_thresh < p.v_spike ) {
		effective_threshold = p.v_thresh;
		LOG4CXX_DEBUG(_log, "using v_thresh = " << p.v_thresh
				<< " instead of v_spike = " << p.v_spike
				<< " for setting the spiking threshold, because delta_T = 0 "
				"and v_thresh < v_spike.");
	}

	try {
		h[HICANN::neuron_parameter::V_t] = to_dac(scaleVoltage(effective_threshold, shiftV, alphaV), Calibrations::V_t);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate v_spike, because: " << e.what());
	}

	LOG4CXX_DEBUG(_log, "v_spike = " << effective_threshold << " mV transformed to V_t = " << h[HICANN::neuron_parameter::V_t]);
}

void NeuronCalibration::setSpikingThreshold(
	PyNNParameters::IF_cond_exp const& p,
	std::vector<hw_value>& h,
	double const shiftV, double const alphaV) const
{
    LOG4CXX_DEBUG(_log, "NeuronCalibration::setSpikingThreshold_IF");
	try {
		h[HICANN::neuron_parameter::V_t] = to_dac(scaleVoltage(p.v_thresh, shiftV, alphaV), Calibrations::V_t);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate v_thresh, because: " << e.what());
	}

	LOG4CXX_DEBUG(_log, "v_thresh = " << p.v_thresh << " mV transformed to V_t = " << h[HICANN::neuron_parameter::V_t]);
}

template<typename CellType>
HWNeuronParameter NeuronCalibration::_applyNeuronCalibration(
	CellType const& p,
	double const speedup,
	NeuronCalibrationParameters const& params) const
{
    LOG4CXX_DEBUG(_log, "NeuronCalibration::applyNeuronCalibration");
	static_assert(
		std::is_same<CellType, PyNNParameters::EIF_cond_exp_isfa_ista>::value ||
		std::is_same<CellType, PyNNParameters::IF_cond_exp>::value,
		"unsuported CellType");


	// BV: check makes no sense. For the ESS, we need a speedup 1!
	//if (speedup<1000 || speedup>100000) {
	//	throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + ": speedup out of range (10^3 < s < 10^5)");
	//}

	check();

	HWNeuronParameter ret;
	std::vector<hw_value>& h = ret.parameters();
	if (h.size() != HICANN::neuron_parameter::__last_neuron) {
		// TODO: use array rather than vector, and remove this check
		throw std::range_error(std::string(__PRETTY_FUNCTION__) + ": NeuronCalibration parameters has incorrect length");
	}

	// LIF dynamics

	// I_gl
	try {
		h[HICANN::neuron_parameter::I_gl] = to_dac(scaleTau(p.tau_m, speedup), params.I_gl());
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate I_gl, because: " << e.what());
		h[HICANN::neuron_parameter::I_gl] = 409;
	}
	LOG4CXX_DEBUG(_log, "tau_m = " << p.tau_m << " ms transformed to I_gl = " << h[HICANN::neuron_parameter::I_gl]);

	// leak a.k.a. rest potential
	try {
		h[HICANN::neuron_parameter::E_l] = to_dac(scaleVoltage(p.v_rest, params.shiftV, params.alphaV), Calibrations::E_l);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate E_l, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "v_rest = " << p.v_rest << " mV transformed to E_l = " << h[HICANN::neuron_parameter::E_l]);

	// excitatory & inhibitory reversal potential
	try {
		h[HICANN::neuron_parameter::E_synx] = to_dac(scaleVoltage(p.e_rev_E, params.shiftV, params.alphaV), Calibrations::E_synx);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate E_synx, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "e_rev_E = " << p.e_rev_E << " mV transformed to E_synx = " << h[HICANN::neuron_parameter::E_synx]);

	try {
		h[HICANN::neuron_parameter::E_syni] = to_dac(scaleVoltage(p.e_rev_I, params.shiftV, params.alphaV), Calibrations::E_syni);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate E_synx, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "e_rev_I = " << p.e_rev_I << " mV transformed to E_syni = " << h[HICANN::neuron_parameter::E_syni]);

	// refractory period
	try {
		h[HICANN::neuron_parameter::I_pl] = to_dac(scaleTau(p.tau_refrac, speedup), Calibrations::I_pl);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate I_pl, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "tau_refrac = " << p.tau_refrac << " ms transformed to I_pl = " << h[HICANN::neuron_parameter::I_pl]);

	// spiking threshold value
	setSpikingThreshold(p, h, params.shiftV, params.alphaV);

	// adaption variables
	setAdaptionParameters(p, h, speedup, params);

	// AdEx - exponential term
	setExponentialTerm(p, h, params.shiftV, params.alphaV);

	// synaptic input
	try {
        h[HICANN::neuron_parameter::V_syntcx] = to_dac(scaleTau(p.tau_syn_E, speedup), Calibrations::V_syntcx);
	} catch (const std::exception& e) {

		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate V_syntcx, because: " << e.what());

		h[HICANN::neuron_parameter::V_syntcx] = 820;

	}
	LOG4CXX_DEBUG(_log, "tau_syn_E = " << p.tau_syn_E << " ms transformed to V_syntcx = " << h[HICANN::neuron_parameter::V_syntcx]);

	try {
        h[HICANN::neuron_parameter::V_syntci] = to_dac(scaleTau(p.tau_syn_I, speedup), Calibrations::V_syntci);
	} catch (const std::exception& e) {

		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate V_syntci, because: " << e.what());

		h[HICANN::neuron_parameter::V_syntci] = 820;

	}
	LOG4CXX_DEBUG(_log, "tau_syn_I = " << p.tau_syn_I << " ms transformed to V_syntci = " << h[HICANN::neuron_parameter::V_syntci]);

	// biases (TECHNICAL PARAMETERS)

	// the transformation of the following parameters is a constant,
	// therefore anyValue is always transformed to that constant value

	// to disable synaptic input set HICANN::neuron_parameter::I_conv(x/i) to 0
	//h[HICANN::neuron_parameter::I_convi]    =    0;
	//h[HICANN::neuron_parameter::I_convx]    =    0;

	try {
		h[HICANN::neuron_parameter::V_convoffi] = to_dac(-1 /*anyValue*/, Calibrations::V_convoffi);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate V_convoffi, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "V_convoffi set to = " << h[HICANN::neuron_parameter::V_convoffi]);

	try {
		h[HICANN::neuron_parameter::V_convoffx] = to_dac(-1 /*anyValue*/, Calibrations::V_convoffx);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate V_convoffx, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "V_convoffx set to = " << h[HICANN::neuron_parameter::V_convoffx]);

	try {
		h[HICANN::neuron_parameter::I_convi] = to_dac(-1 /*anyValue*/, Calibrations::I_convi);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate I_convi, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "I_convi set to = " << h[HICANN::neuron_parameter::I_convi]);

	try {
		h[HICANN::neuron_parameter::I_convx] = to_dac(-1 /*anyValue*/, Calibrations::I_convx);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate I_convx, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "I_convx set to = " << h[HICANN::neuron_parameter::I_convx]);

	try {
		h[HICANN::neuron_parameter::I_intbbi] = to_dac(-1 /*anyValue*/, Calibrations::I_intbbi);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate I_intbbi, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "I_intbbi set to = " << h[HICANN::neuron_parameter::I_intbbi]);

	try {
		h[HICANN::neuron_parameter::I_intbbx] = to_dac(-1 /*anyValue*/, Calibrations::I_intbbx);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate I_intbbx, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "I_intbbx set to = " << h[HICANN::neuron_parameter::I_intbbx]);

	try {
		h[HICANN::neuron_parameter::V_syni] = to_dac(-1 /*anyValue*/, Calibrations::V_syni);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate V_syni, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "V_syni set to = " << h[HICANN::neuron_parameter::V_syni]);

	try {
		h[HICANN::neuron_parameter::V_synx] = to_dac(-1 /*anyValue*/, Calibrations::V_synx);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate V_synx, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "V_synx set to = " << h[HICANN::neuron_parameter::V_synx]);

	try {
		h[HICANN::neuron_parameter::I_spikeamp] = to_dac(-1 /*anyValue*/, Calibrations::I_spikeamp);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: cannot calibrate I_spikeamp, because: " << e.what());
	}
	LOG4CXX_DEBUG(_log, "I_spikeamp set to = " << h[HICANN::neuron_parameter::I_spikeamp]);

    LOG4CXX_DEBUG(_log, "NeuronCalibration::applyNeuronCalibration succesfully applied");

    return ret;
}

// instantiate EIF_cond_exp_isfa_ista calibration
template
HWNeuronParameter NeuronCalibration::_applyNeuronCalibration<PyNNParameters::EIF_cond_exp_isfa_ista>(
	PyNNParameters::EIF_cond_exp_isfa_ista const& p,
	double const speedup,
	NeuronCalibrationParameters const& params) const;

// instantiate IF_cond_exp calibration
template
HWNeuronParameter NeuronCalibration::_applyNeuronCalibration<PyNNParameters::IF_cond_exp>(
	PyNNParameters::IF_cond_exp const& p,
	double const speedup,
	NeuronCalibrationParameters const& params) const;

PyNNParameters::EIF_cond_exp_isfa_ista
NeuronCalibration::applyNeuronReverse(
	HWNeuronParameter const& param,
	double const speedup,
	double const cm_bio,
	NeuronCalibrationParameters const& params) const
{
	LOG4CXX_DEBUG(_log, "NeuronCalibration::applyNeuronReverse");
	auto const& h = param.parameters();

	PyNNParameters::EIF_cond_exp_isfa_ista bio;

	bio.cm = cm_bio;
	LOG4CXX_DEBUG(_log, "Bio-Capacity set to " << bio.cm << " nF");

	//apply inverse transformations

	//leakage conductance
	reverseApplyOne(h[HICANN::neuron_parameter::I_gl], bio.tau_m, params.I_gl());
	bio.tau_m = reverseScaleTau(bio.tau_m, speedup);
	LOG4CXX_DEBUG(_log, "I_gl = " << h[HICANN::neuron_parameter::I_gl] << " transformed to tau_m = " << bio.tau_m << " ms");

	//rest potential
	reverseApplyOne(h[HICANN::neuron_parameter::E_l], bio.v_rest, Calibrations::E_l);
	bio.v_rest = reverseScaleVoltage(bio.v_rest, params.shiftV, params.alphaV);
	LOG4CXX_DEBUG(_log, "E_l = " << h[HICANN::neuron_parameter::E_l] << " transformed to v_rest = " << bio.v_rest << " mV");

	//spinking threshold value
	reverseApplyOne(h[HICANN::neuron_parameter::V_t], bio.v_spike, Calibrations::V_t);
	bio.v_spike = reverseScaleVoltage(bio.v_spike, params.shiftV, params.alphaV);
	LOG4CXX_DEBUG(_log, "V_t = " << h[HICANN::neuron_parameter::V_t] << " transformed to v_spike = " << bio.v_spike << " mV");

	//excitatory and inhibitory reversal potential
	reverseApplyOne(h[HICANN::neuron_parameter::E_synx], bio.e_rev_E, Calibrations::E_synx);
	bio.e_rev_E = reverseScaleVoltage(bio.e_rev_E, params.shiftV, params.alphaV);
	LOG4CXX_DEBUG(_log, "E_synx = " << h[HICANN::neuron_parameter::E_synx] << " transformed to e_rev_E = " << bio.e_rev_E << " mV");
	reverseApplyOne(h[HICANN::neuron_parameter::E_syni], bio.e_rev_I, Calibrations::E_syni);
	bio.e_rev_I = reverseScaleVoltage(bio.e_rev_I, params.shiftV, params.alphaV);
	LOG4CXX_DEBUG(_log, "E_syni = " << h[HICANN::neuron_parameter::E_syni] << " transformed to e_rev_I = " << bio.e_rev_I << " mV");

	//adaptation variables

	//HICANN::neuron_parameter::I_gladapt -> a
	// disable the adaptation for DAC value 0
	if (h[HICANN::neuron_parameter::I_gladapt] > 0)
	{
		reverseApplyOne(h[HICANN::neuron_parameter::I_gladapt], bio.a, params.I_gladapt());
		bio.a = reverseScaleConductance(bio.a, speedup, bio.cm, params.cap());
		LOG4CXX_DEBUG(_log, "I_gladapt = " << h[HICANN::neuron_parameter::I_gladapt] << " transformed to a = " << bio.a << " nS");
	}
	else
	{
		bio.a = 0.;
		LOG4CXX_DEBUG(_log, "I_gladapt = " << h[HICANN::neuron_parameter::I_gladapt] << " set to a = " << bio.a << " nS");
	}
	//HICANN::neuron_parameter::I_fire -> b
	// disable the adaptation for DAC value 0
	if (h[HICANN::neuron_parameter::I_fire] > 0)
	{
		reverseApplyOne(h[HICANN::neuron_parameter::I_fire], bio.b, Calibrations::I_fire);
		bio.b = reverseScaleCurrent(bio.b, speedup, bio.cm, params.alphaV, params.cap());
		LOG4CXX_DEBUG(_log, "I_fire = " << h[HICANN::neuron_parameter::I_fire] << " transformed to b = " << bio.b << " nA");
	}
	else
	{
		bio.b = 0.;
		LOG4CXX_DEBUG(_log, "I_fire = " << h[HICANN::neuron_parameter::I_fire] << " set to b = " << bio.b << " nA");
	}

	if(h[HICANN::neuron_parameter::I_radapt] != 1023) {
		//HICANN::neuron_parameter::I_radapt needs to be inverted before transformation to tau_w
		reverseApplyOne(h[HICANN::neuron_parameter::I_radapt], bio.tau_w, params.I_radapt());
		bio.tau_w = reverseScaleTau(bio.tau_w, speedup);
	} else {
		bio.tau_w = 0;
	}
	LOG4CXX_DEBUG(_log, "I_radapt = " << h[HICANN::neuron_parameter::I_radapt] << " transformed to tau_w = " << bio.tau_w << " ms");

	//exponential term
	reverseApplyOne(h[HICANN::neuron_parameter::V_exp], bio.v_thresh, Calibrations::V_exp);
	bio.v_thresh = reverseScaleVoltage(bio.v_thresh, params.shiftV, params.alphaV);
	LOG4CXX_DEBUG(_log, "V_exp = " << h[HICANN::neuron_parameter::V_exp] << " transformed to v_thresh = " << bio.v_thresh << " mV");
	if(h[HICANN::neuron_parameter::I_rexp] != 1023) {
		reverseApplyOne(h[HICANN::neuron_parameter::I_rexp], bio.delta_T, Calibrations::I_rexp);
		bio.delta_T = reverseScaleVoltageDeltaT(bio.delta_T, params.alphaV);
	} else {
		bio.delta_T = 0;
	}
	LOG4CXX_DEBUG(_log, "I_rexp = " << h[HICANN::neuron_parameter::I_rexp] << " transformed to delta_T = " << bio.delta_T << " mV");

	//refractory period, HICANN::neuron_parameter::I_pl must be inverted before transformation to tau_ref
	reverseApplyOne(h[HICANN::neuron_parameter::I_pl], bio.tau_refrac , Calibrations::I_pl);
	bio.tau_refrac  = reverseScaleTau(bio.tau_refrac, speedup);
	LOG4CXX_DEBUG(_log, "I_pl = " << h[HICANN::neuron_parameter::I_pl] << " transformed to tau_refrac = " << bio.tau_refrac << " ms");

	//synaptic input
	reverseApplyOne(h[HICANN::neuron_parameter::V_syntcx], bio.tau_syn_E, Calibrations::V_syntcx);
	bio.tau_syn_E = reverseScaleTau(bio.tau_syn_E, speedup);
	LOG4CXX_DEBUG(_log, "V_syntcx = " << h[HICANN::neuron_parameter::V_syntcx] << " transformed to tau_syn_E = " << bio.tau_syn_E << " ms");
	reverseApplyOne(h[HICANN::neuron_parameter::V_syntci], bio.tau_syn_I, Calibrations::V_syntci);
	bio.tau_syn_I = reverseScaleTau(bio.tau_syn_I, speedup);
	LOG4CXX_DEBUG(_log, "V_syntci = " << h[HICANN::neuron_parameter::V_syntci] << " transformed to tau_syn_I = " << bio.tau_syn_I << " ms");

	LOG4CXX_DEBUG(_log, "NeuronCalibration::applyNeuronReverse successfully applied");
	return bio;
}

} // HMF

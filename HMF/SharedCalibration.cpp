#include "HMF/SharedCalibration.h"

#include "calibtic/trafo/Polynomial.h"
#include "calibtic/trafo/Constant.h"
#include "calibtic/util.h"

#include <type_traits>
#include "logger.h"

#include "HMF/NeuronCalibration.h"

static log4cxx::LoggerPtr _log = log4cxx::Logger::getLogger("Calibtic");

namespace HMF {

SharedCalibration::SharedCalibration() :
	Calibration(HICANN::shared_parameter::__last_shared)
{
}

SharedCalibration::~SharedCalibration()
{
}

void SharedCalibration::setDefaults()
{
	using namespace calibtic::trafo;
	//check();

	// TODO: check values
	reset(HICANN::shared_parameter::V_reset	   , Polynomial::create({0.0, 1023./1.8}, 0.0, 1.8)); // BV: one-to-one trafo instead of Constant(227);
	reset(HICANN::shared_parameter::int_op_bias, Constant::create(1023));
	reset(HICANN::shared_parameter::V_dllres   , Constant::create(400 ));
	reset(HICANN::shared_parameter::V_bout	   , Constant::create(1023));
	reset(HICANN::shared_parameter::V_bexp	   , Constant::create(1023));
	reset(HICANN::shared_parameter::V_fac	   , Constant::create(0   ));
	reset(HICANN::shared_parameter::I_breset   , Constant::create(1023));
	reset(HICANN::shared_parameter::V_dep	   , Constant::create(0   ));
	reset(HICANN::shared_parameter::I_bstim	   , Constant::create(1023));
	reset(HICANN::shared_parameter::V_thigh	   , Constant::create(0   ));
	reset(HICANN::shared_parameter::V_gmax3	   , Constant::create(500 ));
	reset(HICANN::shared_parameter::V_tlow	   , Constant::create(0   ));
	reset(HICANN::shared_parameter::V_gmax0	   , Constant::create(500 ));
	reset(HICANN::shared_parameter::V_clrc	   , Constant::create(0   ));
	reset(HICANN::shared_parameter::V_clra	   , Constant::create(0   ));
	reset(HICANN::shared_parameter::V_gmax1	   , Constant::create(500 ));
	reset(HICANN::shared_parameter::V_stdf	   , Constant::create(0   ));
	reset(HICANN::shared_parameter::V_gmax2	   , Constant::create(500 ));
	reset(HICANN::shared_parameter::V_m		   , Constant::create(0   ));
	reset(HICANN::shared_parameter::V_bstdf	   , Constant::create(0   ));
	reset(HICANN::shared_parameter::V_dtc	   , Polynomial::create({-1.07638889e+0, 6.31944444e-4})); // see doc/stp_calib.py
	reset(HICANN::shared_parameter::V_br	   , Constant::create(0   ));
	reset(HICANN::shared_parameter::V_ccas	   , Constant::create(800 ));


}

HWSharedParameter SharedCalibration::applySharedCalibration(double v_reset) const {

	HWSharedParameter ret;
	std::vector<hw_value>& h = ret.parameters();

	// FIXME: use a function to convert from mvolt to dac (does exist in NeuronCalibration, should be shared)
	applyOne(v_reset, h[HICANN::shared_parameter::V_reset], HICANN::shared_parameter::V_reset);

	LOG4CXX_DEBUG(_log, "V_reset target [V]: " << v_reset << ", V_reset [DAC]: " << h[HICANN::shared_parameter::V_reset]);



	try {
		applyOne(-1 /*anyValue*/, h[HICANN::shared_parameter::I_breset], HICANN::shared_parameter::I_breset);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: no value retrieved for I_breset, because: " << e.what());
		h[HICANN::shared_parameter::I_breset] = 1023;
	}
	LOG4CXX_DEBUG(_log, "I_breset set to = " << h[HICANN::shared_parameter::I_breset]);

	try {
		applyOne(-1 /*anyValue*/, h[HICANN::shared_parameter::I_bstim], HICANN::shared_parameter::I_bstim);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: no value retrieved for I_bstim, because: " << e.what());
		h[HICANN::shared_parameter::I_bstim] = 1023;
	}
	LOG4CXX_DEBUG(_log, "I_bstim set to = " << h[HICANN::shared_parameter::I_bstim]);

	try {
		applyOne(-1 /*anyValue*/, h[HICANN::shared_parameter::int_op_bias], HICANN::shared_parameter::int_op_bias);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: no value retrieved for int_op_bias, because: " << e.what());
		h[HICANN::shared_parameter::int_op_bias] = 1023;
	}
	LOG4CXX_DEBUG(_log, "int_op_bias set to = " << h[HICANN::shared_parameter::int_op_bias]);

	try {
		applyOne(-1 /*anyValue*/, h[HICANN::shared_parameter::V_bout], HICANN::shared_parameter::V_bout);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: no value retrieved for V_bout, because: " << e.what());
		h[HICANN::shared_parameter::V_bout] = 306;
	}
	LOG4CXX_DEBUG(_log, "V_bout set to = " << h[HICANN::shared_parameter::V_bout]);

	try {
		applyOne(-1 /*anyValue*/, h[HICANN::shared_parameter::V_bexp], HICANN::shared_parameter::V_bexp);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: no value retrieved for V_bexp, because: " << e.what());
		h[HICANN::shared_parameter::V_bexp] = 1023;
	}
	LOG4CXX_DEBUG(_log, "V_bexp set to = " << h[HICANN::shared_parameter::V_bexp]);

	try {
		applyOne(-1 /*anyValue*/, h[HICANN::shared_parameter::V_dllres], HICANN::shared_parameter::V_dllres);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: no value retrieved for V_dllres, because: " << e.what());
		h[HICANN::shared_parameter::V_dllres] = 1023;
	}
	LOG4CXX_DEBUG(_log, "V_dllres set to = " << h[HICANN::shared_parameter::V_dllres]);

	try {
		applyOne(-1 /*anyValue*/, h[HICANN::shared_parameter::V_ccas], HICANN::shared_parameter::V_ccas);
	} catch (const std::exception& e) {
		LOG4CXX_WARN(_log, "Calibtic::NeuronCalibration: no value retrieved for V_ccas, because: " << e.what());
		h[HICANN::shared_parameter::V_ccas] = 600;
	}
	LOG4CXX_DEBUG(_log, "V_ccas set to = " << h[HICANN::shared_parameter::V_ccas]);

	h[HICANN::shared_parameter::V_gmax0] =       50;
	h[HICANN::shared_parameter::V_gmax1] =       50;
	h[HICANN::shared_parameter::V_gmax2] =       50;
	h[HICANN::shared_parameter::V_gmax3] =       50;

	// STDP
	h[HICANN::shared_parameter::V_br] =             0;
	h[HICANN::shared_parameter::V_bstdf] =          0;
	h[HICANN::shared_parameter::V_clrc] =           0;
	h[HICANN::shared_parameter::V_clra] =           0;
	h[HICANN::shared_parameter::V_dep] =            0;
	h[HICANN::shared_parameter::V_dtc] =            0;
	h[HICANN::shared_parameter::V_fac] =            0;
	h[HICANN::shared_parameter::V_m] =              0;
	h[HICANN::shared_parameter::V_stdf] =           0;
	h[HICANN::shared_parameter::V_thigh] =          0;
	h[HICANN::shared_parameter::V_tlow] =           0;

	return ret;

}

HWSharedParameter SharedCalibration::applySharedCalibration(
			HMF::ModelSharedParameter const & p
			) const
{
	// use existing method that only transforms v_reset
	HWSharedParameter ret = applySharedCalibration(p.v_reset);
	std::vector<hw_value>& h = ret.parameters();

	// transform STP parameters.

	// trafo from M=1/T to DAC value for V_dtc.
	// input: M [s^-1]
	int val = round(mTrafo[HICANN::shared_parameter::V_dtc]->apply(1./(p.tau_rec*1e-6)));

	if(val > NeuronCalibration::max_fg_value )
    {
		LOG4CXX_WARN(_log, "digital current value "
		                       << val << " of V_dtc "
		                       << " clipped to maximal floating gate value "
		                       << NeuronCalibration::max_fg_value);
		val = NeuronCalibration::max_fg_value;
    }
	if(val < NeuronCalibration::min_fg_value )
    {
		LOG4CXX_WARN(_log, "digital current value "
		                       << val << " of V_dtc "
		                       << " clipped to minimal floating gate value "
		                       << NeuronCalibration::min_fg_value);
		val = NeuronCalibration::min_fg_value;
    }

	h[HICANN::shared_parameter::V_dtc] = val;

	// stp parameters lambda, N_dep and N_fac
	double const gain = 1.; // [1/V] gain factor of OTA 2 in J.Bills Diploma Thesis
	// voltages in V.
	double V_stdf = p.lambda/gain;
	double V_dep = p.N_dep*V_stdf;
	double V_fac = p.N_fac*V_stdf;
	h[HICANN::shared_parameter::V_stdf] = calibtic::clip( volt_to_dac(V_stdf), NeuronCalibration::min_fg_value, NeuronCalibration::max_fg_value);
	h[HICANN::shared_parameter::V_dep] = calibtic::clip( volt_to_dac(V_dep), NeuronCalibration::min_fg_value, NeuronCalibration::max_fg_value);
	h[HICANN::shared_parameter::V_fac] = calibtic::clip( volt_to_dac(V_fac), NeuronCalibration::min_fg_value, NeuronCalibration::max_fg_value);

	return ret;
}

ModelSharedParameter
SharedCalibration::applySharedReverse(
		HWSharedParameter const & p
		) const
{
	typedef HICANN::shared_parameter hw;

	ModelSharedParameter ret;

	// V_reset
	reverseApplyOne(p.getParam(hw::V_reset), ret.v_reset, hw::V_reset);

	{
		// stp tau_rec
		double one_over_tau_rec_in_s;
		reverseApplyOne(p.getParam(hw::V_dtc), one_over_tau_rec_in_s, hw::V_dtc);
		ret.tau_rec = 1.e6 / one_over_tau_rec_in_s; // convert to micro seconds
	}

	{
		// stp parameters lambda, N_dep and N_fac
		double const gain = 1.; // [1/V] gain factor of OTA 2 in J.Bills Diploma Thesis
		double V_stdf = dac_to_volt( p.getParam(HICANN::shared_parameter::V_stdf) );
		ret.lambda  = gain*V_stdf;
		ret.N_dep = dac_to_volt( p.getParam(HICANN::shared_parameter::V_dep) )/V_stdf;
		ret.N_fac = dac_to_volt( p.getParam(HICANN::shared_parameter::V_fac) )/V_stdf;
	}

	return ret;
}

int SharedCalibration::to_dac(double const v,
	HICANN::shared_parameter const p,
	trafo_t::OutsideDomainBehavior outside_domain_behavior) const {

	double val;
	int returnval = -1;
	applyOne(v, val, p, outside_domain_behavior);
	returnval = round(val);

	//clip the returnvalue
	if(returnval > NeuronCalibration::max_fg_value )
	{
		LOG4CXX_WARN(_log, "digital FG value "
		                       << returnval << " of shared parameter " << p
		                       << " clipped to maximal floating gate value "
		                       << NeuronCalibration::max_fg_value);
		return NeuronCalibration::max_fg_value;
	}
	if(returnval < NeuronCalibration::min_fg_value )
	{
		LOG4CXX_WARN(_log, "digital FG value "
		                       << returnval << " of shared parameter " << p
		                       << " clipped to minimal floating gate value "
		                       << NeuronCalibration::min_fg_value);
		return NeuronCalibration::min_fg_value;
	}
	return returnval;
}

int SharedCalibration::volt_to_dac(double val) {
	return round(val * NeuronCalibration::max_fg_value / NeuronCalibration::max_techn_volt);
}

double SharedCalibration::dac_to_volt(int val) {
	return val * NeuronCalibration::max_techn_volt/NeuronCalibration::max_fg_value;
}

void SharedCalibration::copy(calibtic::Calibration const& rhs)
{
	*this = dynamic_cast<SharedCalibration const&>(rhs);
}

// Py++ factory function
boost::shared_ptr<SharedCalibration> SharedCalibration::create()
{
	return boost::shared_ptr<SharedCalibration>(new SharedCalibration());
}

}

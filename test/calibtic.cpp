#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "test.h"
#include "hal/Coordinate/iter_all.h"

#include "calibtic/Collection.h"
#include "calibtic/Calibration.h"
#include "calibtic/trafo/Transformation.h"
#include "calibtic/trafo/Polynomial.h"
#include "calibtic/trafo/Constant.h"
#include "calibtic/trafo/SumOfTrafos.h"
#include "calibtic/trafo/PowerOfTrafo.h"
#include "calibtic/trafo/InvQuadraticPol.h"
#include "calibtic/trafo/OneOverPolynomial.h"
#include "calibtic/trafo/NegativePowersPolynomial.h"

#include "calibtic/HMF/NeuronCalibration.h"
#include "calibtic/HMF/NeuronCollection.h"
#include "calibtic/HMF/SharedCalibration.h"
#include "calibtic/HMF/SynapseCalibration.h"
#include "calibtic/HMF/SynapseRowCalibration.h"
#include "calibtic/HMF/SynapseRowCollection.h"
#include "calibtic/HMF/L1CrossbarCollection.h"
#include "calibtic/HMF/L1CrossbarCalibration.h"
#include "calibtic/HMF/SynapseChainLengthCollection.h"
#include "calibtic/HMF/SynapseChainLengthCalibration.h"

using namespace calibtic;
using namespace calibtic::trafo;
using namespace PyNNParameters;
using boost::shared_ptr;

template<typename T>
class BasicTest : public TestWithBackend<T> {};
TYPED_TEST_CASE(BasicTest, BackendTypes);

TYPED_TEST(BasicTest, NeuronCalibration)
{
	HMF::NeuronCollection set0;
	shared_ptr<HMF::NeuronCalibration> setter(new HMF::NeuronCalibration());
	setter->setDefaults();
	set0.insert(0, setter);

	TestFixture::backend->store("fisch", MetaData(), set0);

	MetaData md;
	HMF::NeuronCollection set1;
	TestFixture::backend->load("fisch", md, set1);
	ASSERT_EQ(set0, set1);

	shared_ptr<HMF::NeuronCalibration> getter =
		boost::dynamic_pointer_cast<HMF::NeuronCalibration>(set1.at(0));

	// test type (unsuccessful cast returns nullptr)
	ASSERT_NE(getter, shared_ptr<HMF::NeuronCalibration>());
	ASSERT_EQ(getter->size(), setter->size());

	// NOTE: don't forget to initialize parameters with default values,
	// otherwise they'll contain anything.
	EIF_cond_exp_isfa_ista bio;

	HMF::HWNeuronParameter hw0 = set0.applyNeuronCalibration(bio, 0);
	HMF::HWNeuronParameter hw1 = set1.applyNeuronCalibration(bio, 0);

	for (size_t ii = 0; ii < hw0.size(); ++ii) {
		EXPECT_EQ(hw0.getParam(ii), hw1.getParam(ii));
		if (hw0.getParam(ii) != hw1.getParam(ii))
			std::cout << ii << std::endl;
	}
}

TYPED_TEST(BasicTest, NeuronCollection)
{
	MetaData md;
	HMF::NeuronCollection set0;
	set0.setSpeedup(1337);
	TestFixture::backend->store("fisch", md, set0);

	for (size_t ii=0; ii<2; ii++) {
		HMF::NeuronCollection set1;
		TestFixture::backend->load("fisch", md, set1);
		ASSERT_EQ(1337, set1.getSpeedup());
	}
}

class TestableCalibration : public Calibration
{
public:
	TestableCalibration(size_type const size = 0, value_type const value = value_type()) :
		Calibration(size, value) {};

	using Calibration::applyOne;
	using Calibration::apply;
};

TEST(Calibtic, Base)
{
	Collection set;

	shared_ptr<Calibration> setter(new Calibration);
	set.insert(0, setter);

	double val;
	TestableCalibration cal(1);
	EXPECT_EQ(cal.size(), 1);
	EXPECT_ANY_THROW( cal.applyOne(2.3, val); );
	cal.reset(0, Constant::create(2.5));
	ASSERT_EQ(cal.size(), 1);
	ASSERT_NO_THROW( cal.applyOne(2.3, val); );
	EXPECT_EQ(val, 2.5);

	std::vector<double> in{1.0, 1.0, 2.0};
	std::vector<double> out;
	cal = TestableCalibration(3, Constant::create(2.5));
	ASSERT_EQ(cal.size(), 3);
	ASSERT_NO_THROW( cal.applyOne(2.3, val); );
	EXPECT_EQ(val, 2.5);
	ASSERT_NO_THROW( cal.apply(in.begin(), in.end(), std::back_inserter(out)); );
	for (auto const& d : out) {
		EXPECT_EQ(d, 2.5);
	}

	//std::stringstream sstr;
	//boost::archive::xml_oarchive oa(sstr);

	//oa << boost::serialization::make_nvp("test", set);
}

TEST(CalibticTransformation, Polynomial)
{
	Polynomial p({1, 0.5}), p0, p2({1,2,3});
	ASSERT_EQ(p.apply(5), 3.5);
	ASSERT_EQ(p.apply(5.), 3.5);
	ASSERT_EQ(p0.apply(5), 0.);
	ASSERT_EQ(p0.apply(5.), 0.);
	ASSERT_EQ(p2.apply(2), 17.);
	ASSERT_EQ(p2.apply(2.), 17.);
}

TEST(CalibticTransformation, PolynomialReverse)
{
	// Test linear and quadratic polynomial
	srand(1);
	Polynomial p({1, 0.5}), p2({1,2,3}, 0, 50);

	REPEAT(20)
	{
		double val = (rand() % 500)*0.1; // between 0 and 50
		// Calculate the right domain for random values for reverseApply
		int reverse_min = p2.apply(0);
		int reverse_max = p2.apply(50);
		double reverse_val = (rand() % 10*(reverse_max-reverse_min) + 10*reverse_min)*0.1;
		ASSERT_NEAR(p.reverseApply(p.apply(val)), val, 0.001);
		ASSERT_NEAR(p.apply(p.reverseApply(val)), val, 0.001);
		ASSERT_NEAR(p2.reverseApply(p2.apply(val)), val, 0.001);
		ASSERT_NEAR(p2.apply(p2.reverseApply(reverse_val)), reverse_val, 0.001);
	}
}

TEST(CalibticTransformation, Constant)
{
	Constant c(2.5), c0;
	ASSERT_EQ(c.apply(5), 2.5);
	ASSERT_EQ(c.apply(5.), 2.5);
	ASSERT_EQ(c0.apply(5), 0.);
	ASSERT_EQ(c0.apply(5.), 0.);
}

TEST(CalibticTransformation, InvQuadraticPol)
{
	double const epsilon = 0.01;
	InvQuadraticPol::data_type a{{-2, 4, 1, 2 }};
	InvQuadraticPol s(a);
	ASSERT_NEAR(1.6458, s.apply(5), epsilon);
	ASSERT_NEAR(1.6458, s.apply(5.), epsilon);
	ASSERT_NEAR(1, s.apply(2), epsilon);
	ASSERT_NEAR(1., s.apply(2.), epsilon);
}

TEST(CalibticTransformation, OneOverPolynomial)
{
	double const epsilon = 0.01;
	OneOverPolynomial::data_type a1{{5.}};
	OneOverPolynomial o1(a1);
	ASSERT_EQ(o1.apply(0), 0.2);
	ASSERT_EQ(o1.apply(1), 0.2);

	OneOverPolynomial::data_type a2{{5., 0}};
	OneOverPolynomial o2(a2);
	ASSERT_EQ(o2.apply(0), 0.2);
	ASSERT_EQ(o2.apply(1), 0.2);

	OneOverPolynomial::data_type a3{{5., 1}};
	OneOverPolynomial o3(a3);
	ASSERT_NEAR(0.166, o3.apply(1), epsilon);
	ASSERT_NEAR(0.1428, o3.apply(2), epsilon);
}

TEST(CalibticTransformation, OneOverPolynomialReverse)
{
	// Test linear and quadratic polynomial
	srand(1);
	OneOverPolynomial p({1, 0.5}), p2({1,2,3}, 0, 50);

	double const eps = 0.001;

	REPEAT(20)
	{
		double const val = (rand() % 500)*0.1; // between 0.x and 50.x
		double const reverse_val_p = rand()%9*0.1+0.1; // > 0.1 and smaller than 1
		ASSERT_NEAR(p.reverseApply(p.apply(val)), val, eps);
		ASSERT_NEAR(p.apply(p.reverseApply(reverse_val_p)), reverse_val_p, eps);

		// Calculate the right domain for random values for reverseApply
		int const reverse_min = p2.apply(0);
		int const reverse_max = p2.apply(50);
		double const reverse_val_p2 = (rand() % 10*(reverse_max-reverse_min) + 10*reverse_min)*0.1;
		ASSERT_NEAR(p2.reverseApply(p2.apply(val)), val, eps);
		ASSERT_NEAR(p2.apply(p2.reverseApply(reverse_val_p2)), reverse_val_p2, eps);
	}
}

TEST(CalibticTransformation, NegativePowersPolynomial)
{
	double const epsilon = 0.01;
	NegativePowersPolynomial::data_type a1{{5.}};
	NegativePowersPolynomial o1(a1);
	ASSERT_EQ(o1.apply(0), 5.);
	ASSERT_EQ(o1.apply(1), 5.);

	NegativePowersPolynomial::data_type a2{{1., 5.}};
	NegativePowersPolynomial o2(a2);
	ASSERT_NEAR(6, o2.apply(1), epsilon);
	ASSERT_NEAR(3.5, o2.apply(2), epsilon);
}

TEST(CalibticTransformation, SumOfTrafos)
{
	double const epsilon = 0.01;

	SumOfTrafos::trafo_list trafos_const = {
	    boost::shared_ptr<Transformation>(new Constant(5)),
	    boost::shared_ptr<Transformation>(new Constant(7))};

	SumOfTrafos o1(trafos_const);
	ASSERT_EQ(o1.apply(0), 12.);

	SumOfTrafos::trafo_list trafos_mix = {
	    boost::shared_ptr<Transformation>(new NegativePowersPolynomial({1., 5.})),
	    boost::shared_ptr<Transformation>(new Constant(7))};

	SumOfTrafos o2(trafos_mix);
	ASSERT_NEAR(o2.apply(2), 10.5, epsilon);

	SumOfTrafos::trafo_list trafos_mix_2 = {
	    boost::shared_ptr<Transformation>(new NegativePowersPolynomial({1., 5.})),
	    boost::shared_ptr<Transformation>(
	        new InvQuadraticPol(InvQuadraticPol::data_type{{-2, 4, 1, 2}}))};

	SumOfTrafos o3(trafos_mix_2);
	ASSERT_NEAR(o3.apply(2), 4.5, epsilon);

}

TEST(CalibticTransformation, NegativePowersPolynomialReverse)
{
	// Test linear and quadratic polynomial
	srand(1);
	NegativePowersPolynomial p({1, 0.5}), p2({1,2,3}, 0.1, 50);

	double const eps = 0.001;

	REPEAT(20)
	{
		double const val = (rand() % 500)*0.1; // between 0.x and 50.x
		// Calculate the right domain for random values for reverseApply
		int const reverse_min = p2.apply(0.1);
		int const reverse_max = p2.apply(50);
		double const reverse_val = (rand() % 10*(reverse_max-reverse_min) + 10*reverse_min)*0.1;
		ASSERT_NEAR(p.reverseApply(p.apply(val)), val, eps);
		ASSERT_NEAR(p.apply(p.reverseApply(val)), val, eps);
		ASSERT_NEAR(p2.reverseApply(p2.apply(val)), val, eps);
		ASSERT_NEAR(p2.apply(p2.reverseApply(reverse_val)), reverse_val, eps);
	}
}

TEST(CalibticTransformation, PowerOfTrafo)
{

	double const eps = 0.001;

	PowerOfTrafo p(0.5, boost::shared_ptr<Transformation>(new Constant(5)));
	ASSERT_NEAR(p.apply(1), 2.2360679, eps);

	OneOverPolynomial::data_type a3{{5., 1}};
	PowerOfTrafo p3(3, boost::shared_ptr<Transformation>(new OneOverPolynomial(a3)));
	ASSERT_NEAR(0.002911954752, p3.apply(2), eps);

}

// transform PyNN parameters to DAC values, compare
// with results calculated manually
TEST(Calibtic, CalibBioToHw)
{
	using namespace HMF;
	typedef HICANN::neuron_parameter h;

	NeuronCalibration mCalib;
	mCalib.setDefaults();

	//generation of random data not possible, therefore values within sensible
	//bio regime chosen (cf. Thesis M. O. Schwartz page 25)
	//tau_syn_E/I adapted to v4 default curve (HMF/v_syntc_ideal.dat)
	//taken from ideal simulations
	PyNNParameters::EIF_cond_exp_isfa_ista inp;
	inp.tau_refrac = 1.00;
	inp.a =          2.5;
	inp.tau_m =      10.0;
	inp.e_rev_E =    0.0;
	inp.cm =         0.24;
	inp.delta_T =    1.2;
	inp.e_rev_I =    -80.0;
	inp.v_thresh =   -40.0;
	inp.b =          0.05;
	inp.tau_syn_E =  2;
	inp.v_spike =    0.0;
	inp.tau_syn_I =  2;
	inp.tau_w =      30.0;
	inp.v_rest =     -60.0;

	double const speedup = 10000.;
	NeuronCalibrationParameters nc_params;
	nc_params.shiftV = 1.2;
	nc_params.alphaV = 10;
	HWNeuronParameter calibrated = mCalib.applyNeuronCalibration(inp, speedup, nc_params);

	//results calculated in calibtic/doc/BioToHW_tests.py
	EXPECT_EQ( calibrated.getParam(h::I_pl), 102);
	EXPECT_EQ( calibrated.getParam(h::I_gladapt), 25);
	EXPECT_EQ( calibrated.getParam(h::I_gl), 189);
	EXPECT_EQ( calibrated.getParam(h::E_syni), 227);
	EXPECT_EQ( calibrated.getParam(h::E_synx), 721);
	EXPECT_EQ( calibrated.getParam(h::I_rexp), 832);
	EXPECT_EQ( calibrated.getParam(h::I_fire), 736);
	EXPECT_EQ( calibrated.getParam(h::V_syntcx), 481);
	EXPECT_EQ( calibrated.getParam(h::V_syntci), 481);
	EXPECT_EQ( calibrated.getParam(h::V_t), 682);
	EXPECT_EQ( calibrated.getParam(h::I_radapt), 819);
	EXPECT_EQ( calibrated.getParam(h::E_l), 341);
	EXPECT_EQ( calibrated.getParam(h::V_exp), 455);
}

TEST(Calibtic, CalibHwtoBio)
{
	using namespace HMF;
	typedef HICANN::neuron_parameter h;

	NeuronCalibration mCalib;
	mCalib.setDefaults();

	NeuronCalibrationParameters mCalibParams;
	mCalibParams.alphaV = 10.;
	mCalibParams.shiftV = 1.2;

	//test data
	HWNeuronParameter inp;
	inp.setParam(h::E_l,      511);
	inp.setParam(h::I_gl,     511);
	inp.setParam(h::I_gladapt,511);
	inp.setParam(h::I_radapt, 511);
	inp.setParam(h::I_rexp,   511);
	inp.setParam(h::I_fire,   511);
	inp.setParam(h::E_synx,   511);
	inp.setParam(h::E_syni,   511);
	inp.setParam(h::V_syntcx, 487);
	inp.setParam(h::V_syntci, 487);
	inp.setParam(h::V_exp,    511);
	inp.setParam(h::I_pl,     511);
	inp.setParam(h::V_t,      511);

	double const speedup = 10000;
	double const cm_bio = 100. * HMF::NeuronCalibrationParameters::big_cap;
	auto bio = mCalib.applyNeuronReverse(inp, speedup, cm_bio, mCalibParams);

	//accepted relative error
	double eps = 0.001;

	EXPECT_NEAR( -30.088   , bio.v_rest     , eps);
	EXPECT_NEAR( 6.49585   , bio.tau_m      , eps);
	EXPECT_NEAR( 30.4612   , bio.a          , eps);
	EXPECT_NEAR( 38.6019   , bio.tau_w      , eps);
	EXPECT_NEAR( 0.89882   , bio.delta_T    , eps);
	EXPECT_NEAR( 0.02918   , bio.b          , eps);
	EXPECT_NEAR( -34.1693  , bio.e_rev_E    , eps);
	EXPECT_NEAR( -30.0879  , bio.e_rev_I    , eps);

	//tau_syn_E/I adapted to v4 averaged curve (HMF/v_syntcx/i_averaged.dat)
	//we need a higher relative error here because the curve is highly
	//non-linear
	EXPECT_NEAR( 1.96297   , bio.tau_syn_E  , eps*2);
	EXPECT_NEAR( 1.96297   , bio.tau_syn_I  , eps*2);

	EXPECT_NEAR( -30.088   , bio.v_thresh   , eps);
	EXPECT_NEAR( 0.28240   , bio.tau_refrac , eps);
	EXPECT_NEAR( -30.088   , bio.v_spike    , eps);
}

#define CHECK_TRAFO(input, output, param, eps) \
	{ \
		double const in = input . param; \
		double const out = output . param ; \
		EXPECT_NEAR(in, out, eps); \
	}

// disabled, see issue 1461
TEST(Calibtic, CalibBioToHwToBio)
{
	using namespace HMF;
	typedef HICANN::neuron_parameter h;

	NeuronCalibration mCalib;
	mCalib.setDefaults();

	srand(2);

	REPEAT(10)
	{
		//generate random test data with sensible values (cf. Thesis M. O.
		//Schwartz page 25)
		//tau_syn_E/I adapted to v4 default curve (HMF/v_syntc_ideal.dat)
		//taken from ideal simulations
		PyNNParameters::EIF_cond_exp_isfa_ista inp;
	    inp.tau_refrac = (rand() % 480)*0.01 + 0.2; // from 0.2x to 5.xx
	    inp.a =          (rand() % 320)*0.01 + 0.8; // from 0.8x to 4.xx
	    inp.tau_m =      (rand() % 1110)*0.01 + 4.9; // from 4.9x to 15.xx
	    inp.e_rev_E =    (rand() % 480)*0.01 + 0.2; // from 0.2x to 5.xx
	    inp.cm =         0.216456;
	    inp.delta_T =    (rand() % 120)*0.01 + 0.2; // from 0.2x to 1.4x
	    inp.e_rev_I =    (rand() % 500)*0.01 - 80; // from -80.xx to -75.xx
	    inp.v_thresh =   (rand() % 600)*0.01 - 56; // from -56.xx to -42.xx
	    inp.b =          (rand() % 69)*0.001; // from 0.000 to 0.069
	    inp.tau_syn_E =  (rand() % 1000)*0.01 + 1;  // from 15.xx to 20.xx
	    inp.v_spike =    (rand() % 480)*0.01 + 0.2; // from 0.2x to 5.xx
	    inp.tau_syn_I =  (rand() % 1000)*0.01 + 1 ; // from 1.xx to 20.xx
	    inp.tau_w =      (rand() % 9000)*0.01 + 40; // from 40.xx to 130.
	    inp.v_rest =     (rand() % 1200)*0.01 - 70; // from -70.xx to -58

	    double const speedup = 10000.;
	    HWNeuronParameter calibrated = mCalib.applyNeuronCalibration(inp, speedup);

		PyNNParameters::EIF_cond_exp_isfa_ista outp = mCalib.applyNeuronReverse(calibrated, speedup, inp.cm);

		if (inp.a != 0)
		{
			// reverse requires I_gladapt > 0
			EXPECT_GT(calibrated.getParam(h::I_gladapt), 0);
		}
		if (inp.b != 0)
		{
			// reverse requires I_fire > 22
			EXPECT_GT(calibrated.getParam(h::I_fire), 22);
		}

		double eps = 1.0;

	    //check if params match
		CHECK_TRAFO(inp, outp, tau_refrac, eps)
		CHECK_TRAFO(inp, outp, a, eps)
		CHECK_TRAFO(inp, outp, tau_m, eps)
		CHECK_TRAFO(inp, outp, e_rev_E, eps)
		CHECK_TRAFO(inp, outp, e_rev_I, eps)
		CHECK_TRAFO(inp, outp, cm, eps)
		CHECK_TRAFO(inp, outp, delta_T, eps)
		CHECK_TRAFO(inp, outp, v_thresh, eps)
		CHECK_TRAFO(inp, outp, b, eps)
		CHECK_TRAFO(inp, outp, tau_syn_E, eps)
		CHECK_TRAFO(inp, outp, tau_syn_I, eps)
		CHECK_TRAFO(inp, outp, v_spike, eps)
		CHECK_TRAFO(inp, outp, tau_w, eps)
		CHECK_TRAFO(inp, outp, v_rest, eps)
	}
}


// disabled, see issue 1461
TEST(Calibtic, DISABLED_CalibBioToHwToBio2)
{
	using namespace HMF;
	typedef HICANN::neuron_parameter h;

	// debug output
	//logger_default_config(log4cxx::Level::getDebug());

	NeuronCalibration mCalib;
	mCalib.setDefaults();

	NeuronCalibrationParameters mCalibParams;
	mCalibParams.alphaV = 10.;
	mCalibParams.shiftV = 1.2;

	double const speedup = 10000.;

	PyNNParameters::EIF_cond_exp_isfa_ista in_min, in_max;
	in_min.tau_refrac = 0.2;
	in_min.a =          -0.8;
	in_min.tau_m =      4.9;
	in_min.e_rev_E =    0.2;
	in_min.cm =         0.216456;
	in_min.delta_T =    0.2;
	in_min.e_rev_I =    -80.;
	in_min.v_thresh =   -56. ;
	in_min.b =          0.;
	in_min.tau_syn_E =  5.;
	in_min.v_spike =    0.2;
	in_min.tau_syn_I =  5.;
	in_min.tau_w =      20;
	in_min.v_rest =     -70.;

	in_max.tau_refrac = 5.;
	in_max.a =          4.;
	in_max.tau_m =      15; //tau = c/g in g = 1.7,...,18 [nS]; c = 0.24 [nF]
	in_max.e_rev_E =    5.;
	in_max.cm =         0.216456;
	in_max.delta_T =    1.4;
	in_max.e_rev_I =    -75.;
	in_max.v_thresh =   -42.;
	in_max.b =          0.069; //nA instead of pA
	in_max.tau_syn_E =  15.;
	in_max.v_spike =    5.;
	in_max.tau_syn_I =  15.;
	in_max.tau_w =      300.;
	in_max.v_rest =     -58.;

	// debug output
	//std::cout << "Min:\n" << mCalib.applyNeuronCalibration(in_min, speedup)
	//	      << "\nMax:\n" << mCalib.applyNeuronCalibration(in_max, speedup)
	//	      << std::endl;

	REPEAT(50)
	{
		//generate random test data with sensible values (cf. Thesis M. O.
		//Schwartz page 25)
		PyNNParameters::EIF_cond_exp_isfa_ista inp, outp;
		HWNeuronParameter calibrated;

	    inp.tau_refrac = random<double>(in_min.tau_refrac, in_max.tau_refrac);
	    inp.a =          random<double>(in_min.a, in_max.a);
	    inp.tau_m =      random<double>(in_min.tau_m, in_max.tau_m);
	    inp.e_rev_E =    random<double>(in_min.e_rev_E, in_max.e_rev_E);
	    inp.cm =         0.216456;
	    inp.delta_T =    random<double>(in_min.delta_T, in_max.delta_T);
	    inp.e_rev_I =    random<double>(in_min.e_rev_I, in_max.e_rev_I);
	    inp.v_thresh =   random<double>(in_min.v_thresh, in_max.v_thresh);
	    inp.b =          random<double>(in_min.b, in_max.b);
	    inp.tau_syn_E =  random<double>(in_min.tau_syn_E, in_max.tau_syn_E);
	    inp.v_spike =    random<double>(in_min.v_spike, in_max.v_spike);
	    inp.tau_syn_I =  random<double>(in_min.tau_syn_I, in_max.tau_syn_I);
	    inp.tau_w =      random<double>(in_min.tau_w, in_max.tau_w);
	    inp.v_rest =     random<double>(in_min.v_rest, in_max.v_rest);

	    ASSERT_NO_THROW(calibrated =
				mCalib.applyNeuronCalibration(inp, speedup, mCalibParams))
			<< "for input parameters: " << inp;

		outp = mCalib.applyNeuronReverse(calibrated, speedup, inp.cm, mCalibParams);

		//ASSERT_NO_THROW(outp = mCalib.applyNeuronReverse(calibrated, speedup, inp.cm, mCalibParams))
		//	<< "for input parameters: " << inp
		//	<< "\n to: " << calibrated;

		if (inp.a != 0)
		{
			// reverse requires I_gladapt > 0
			EXPECT_GT(calibrated.getParam(h::I_gladapt), 0);
		}
		if (inp.b != 0)
		{
			// reverse requires I_fire > 22
			EXPECT_GT(calibrated.getParam(h::I_fire), 22);
		}

		double eps = 1.0;

	    //check if params match
		CHECK_TRAFO(inp, outp, tau_refrac, eps)
		CHECK_TRAFO(inp, outp, a, eps)
		CHECK_TRAFO(inp, outp, tau_m, eps)
		CHECK_TRAFO(inp, outp, e_rev_E, eps)
		CHECK_TRAFO(inp, outp, e_rev_I, eps)
		CHECK_TRAFO(inp, outp, cm, eps)
		CHECK_TRAFO(inp, outp, delta_T, eps)
		CHECK_TRAFO(inp, outp, v_thresh, eps)
		CHECK_TRAFO(inp, outp, b, eps)
		CHECK_TRAFO(inp, outp, tau_syn_E, eps)
		CHECK_TRAFO(inp, outp, tau_syn_I, eps)
		CHECK_TRAFO(inp, outp, v_spike, eps)
		CHECK_TRAFO(inp, outp, tau_w, eps)
		CHECK_TRAFO(inp, outp, v_rest, eps)
	}
}

/// tests special cases of transformation of AdEx model
TEST(Calibtic, NeuronCalibAdExSpecialCases)
{
	using namespace HMF;
	typedef HICANN::neuron_parameter h;

	NeuronCalibration mCalib;
	mCalib.setDefaults();

	double const speedup = 10000;
	NeuronCalibrationParameters nc_params;

	// disable sub-threshold adaptation (a=0)
	{
		PyNNParameters::EIF_cond_exp_isfa_ista bio;
		bio.a = 0.;

		HWNeuronParameter hw = mCalib.applyNeuronCalibration(bio, speedup, nc_params);

		ASSERT_EQ(0, hw.getParam(h::I_gladapt)); // sub-threshold adaptation disabled
		ASSERT_NE(0, hw.getParam(h::I_fire)); // spike-triggered adaptation not

		auto bio2 = mCalib.applyNeuronReverse(hw, speedup, bio.cm, nc_params);

		ASSERT_EQ(0., bio2.a); // retransforms to 0.
	}

	// disable spike triggered adaptation (b=0)
	{
		PyNNParameters::EIF_cond_exp_isfa_ista bio;
		bio.b = 0.;

		HWNeuronParameter hw = mCalib.applyNeuronCalibration(bio, speedup, nc_params);

		ASSERT_EQ(0, hw.getParam(h::I_fire)); // spike-triggered adaptation disabled
		ASSERT_NE(0, hw.getParam(h::I_gladapt)); // sub-threshold adaptation not

		auto bio2 = mCalib.applyNeuronReverse(hw, speedup, bio.cm, nc_params);

		ASSERT_EQ(0., bio2.b); // retransforms to 0.
	}

	// disable exponential term (delta_T=0)
	// Case 1: v_thresh > v_spike
	{
		PyNNParameters::EIF_cond_exp_isfa_ista bio;
		bio.delta_T = 0.;
		bio.v_thresh = bio.v_spike + 10.;

		HWNeuronParameter hw = mCalib.applyNeuronCalibration(bio, speedup, nc_params);

		// hw params disabling exp term
		ASSERT_EQ(1023, hw.getParam(h::V_exp));
		ASSERT_EQ(1023, hw.getParam(h::I_rexp));
		ASSERT_EQ(1023, hw.getParam(h::I_bexp));

		// assure that v_spike was used for setting V_t
		// by comparing it to the normal trafo.
		PyNNParameters::EIF_cond_exp_isfa_ista bio_default;
		HWNeuronParameter hw_default = mCalib.applyNeuronCalibration(bio_default, speedup, nc_params);

		ASSERT_EQ(hw_default.getParam(h::V_t), hw.getParam(h::V_t));

		auto bio2 = mCalib.applyNeuronReverse(hw, speedup, bio.cm, nc_params);

		ASSERT_EQ(0., bio2.delta_T); // retransforms to 0.
	}

	// Case 2: v_thresh < v_spike
	{
		PyNNParameters::EIF_cond_exp_isfa_ista bio;
		bio.delta_T = 0.;
		bio.v_thresh = bio.v_spike - 10.;

		HWNeuronParameter hw = mCalib.applyNeuronCalibration(bio, speedup, nc_params);

		// hw params disabling exp term
		ASSERT_EQ(1023, hw.getParam(h::V_exp));
		ASSERT_EQ(1023, hw.getParam(h::I_rexp));
		ASSERT_EQ(1023, hw.getParam(h::I_bexp));

		// assure that v_thresh was used for setting V_t
		// by comparing it to the normal trafo: it should be smaller
		PyNNParameters::EIF_cond_exp_isfa_ista bio_default;
		HWNeuronParameter hw_default = mCalib.applyNeuronCalibration(bio_default, speedup, nc_params);

		ASSERT_GT(hw_default.getParam(h::V_t), hw.getParam(h::V_t));

		auto bio2 = mCalib.applyNeuronReverse(hw, speedup, bio.cm, nc_params);

		ASSERT_EQ(0., bio2.delta_T); // retransforms to 0.
	}
}

/// checks for the default transformations, whether the boundaries are ok,
/// First retrieve the boundaries, then apply the transformation, finally
/// try to transform back via reverseApply.
/// This is repeated with hw values retrieved via NeuronCalibraiont.to_dac()
TEST(Calibtic, CheckBoundariesOfDefaultNeuronCalib)
{
	using namespace HMF;
	typedef NeuronCalibrationParameters::Calibrations::calib c;

	std::vector<c> params = {
	    c::I_pl,                     c::V_t,                           c::E_l,
	    c::I_gl_slow0_fast0_bigcap0, c::I_gladapt_slow0_fast0_bigcap0, c::I_fire,
	    c::I_rexp,                   c::I_radapt_slow0_fast0_bigcap0,  c::V_exp,
	    c::E_synx,                   c::V_syntcx,                      c::E_syni,
	    c::V_syntci};

	NeuronCalibration mCalib;
	mCalib.setDefaults();

	for (auto param : params ) {
		// cast to polynomial to access boundaries.
		shared_ptr<Polynomial> trafo =
			boost::dynamic_pointer_cast<Polynomial>(mCalib.at(param));
		if (trafo == nullptr) {
			continue;
		}
		auto const boundaries = trafo->getDomainBoundaries();

		// reverseApply with double
		{
			std::vector<double> hw_boundaries;

			hw_boundaries.push_back(mCalib.at(param)->apply(boundaries.first));
			hw_boundaries.push_back(mCalib.at(param)->apply(boundaries.second));

			std::sort(hw_boundaries.begin(), hw_boundaries.end());

			static const double eps = 0.001;
			double hw_min = hw_boundaries[0] + eps;
			double hw_max = hw_boundaries[1] - eps;

			EXPECT_NO_THROW( mCalib.at(param)->reverseApply(hw_min) )
				<< "Reverse Trafo fails for param " << param << ", hw " << hw_min;

			EXPECT_NO_THROW( mCalib.at(param)->reverseApply(hw_max) )
				<< "Reverse Trafo fails for param " << param << ", hw " << hw_max;
		}

		// reverseApply with int using to_dac(..)
		{
			std::vector<int> dac_boundaries;

			dac_boundaries.push_back(mCalib.to_dac(boundaries.first,param));
			dac_boundaries.push_back(mCalib.to_dac(boundaries.second,param));

			// In NeuronCalibration.to_dac doubles are rounded to ints.
			// We account for that by adding +1(-1) to the lower(upper) bound
			std::sort(dac_boundaries.begin(), dac_boundaries.end());

			int dac_min = dac_boundaries[0] + 1;
			int dac_max = dac_boundaries[1] - 1;

			EXPECT_NO_THROW( mCalib.at(param)->reverseApply(dac_min) )
				<< "Reverse Trafo fails for param " << param << ", dac " << dac_min;

			EXPECT_NO_THROW( mCalib.at(param)->reverseApply(dac_max) )
				<< "Reverse Trafo fails for param " << param << ", dac " << dac_max;
		}
	}
}

TEST(Calibtic, SharedCalibrationVReset)
{
	using namespace HMF;
	typedef HICANN::shared_parameter h;

	SharedCalibration sc;
	sc.setDefaults();

	REPEAT(20)
	{
		// default V_reset trafo: (0, 1.8) V is mapped to (0, 1023) DAC values
		double v_reset = random<double>(0., 1.8); // Volt
		size_t const expected =
		    v_reset * NeuronCalibration::max_fg_value / NeuronCalibration::max_techn_volt;

		// 1st version
		auto hw0 = sc.applySharedCalibration(v_reset);
		EXPECT_EQ(hw0.getParam(h::V_reset), expected);

		// 2nd version
		ModelSharedParameter m_in;
		m_in.v_reset = v_reset;
		auto hw1 = sc.applySharedCalibration(m_in);
		EXPECT_EQ(hw1.getParam(h::V_reset), expected);

		// Reverse Transformation
		auto m_out = sc.applySharedReverse(hw1);

		// accepted absolute error
		double const eps = 1.8 / 1000; // slightly more than 1 DAC

		EXPECT_NEAR(m_in.v_reset, m_out.v_reset, eps);
	}
}

TEST(Calibtic, SharedCalibrationSTP)
{
	using namespace HMF;

	SharedCalibration sc;
	sc.setDefaults();

	// Test two different scnenarios,
	// for details see Petrovici et al. 2014, Appendix S1

	// depression (tau_rec=100ms, U=0.2, tau_facil=0)
	ModelSharedParameter m_dep;
	m_dep.tau_rec = 10.; // us
	m_dep.lambda = 1.0;
	m_dep.N_dep = 0.0;
	m_dep.N_fac = 1.0;

	// facilitation (tau_facil=50ms, U=0.15, tau_rec=0)
	ModelSharedParameter m_fac;
	m_fac.tau_rec = 5.; // us
	m_fac.lambda = 1.0;
	m_fac.N_dep = 0.0;
	m_fac.N_fac = (1 - 2 * 0.15) / (1 - 0.15);

	for (auto m_in : std::vector<ModelSharedParameter>{m_dep, m_fac}) {
		// Test trafo to hardware and back to model
		auto hw1 = sc.applySharedCalibration(m_in);
		auto m_out = sc.applySharedReverse(hw1);

		EXPECT_NEAR(m_in.tau_rec, m_out.tau_rec, 0.5);
		EXPECT_NEAR(m_in.lambda, m_out.lambda, 0.05);
		EXPECT_NEAR(m_in.N_dep, m_out.N_dep, 0.05);
		EXPECT_NEAR(m_in.N_fac, m_out.N_fac, 0.05);
	}
}

TEST(Calibtic, SynapseCalibration)
{
	using namespace HMF;
	SynapseCalibration sc;
	sc.setDefaults();
	ASSERT_EQ(1,sc.size());
	sc.getMinAnalogWeight();
	sc.getMaxAnalogWeight();
	for (size_t dw = HICANN::SynapseWeight::min; dw <= HICANN::SynapseWeight::max; ++dw) {
		ASSERT_NO_THROW(sc.getAnalogWeight(HICANN::SynapseWeight(dw)));
		double aw = sc.getAnalogWeight(HICANN::SynapseWeight(dw));
		ASSERT_EQ(HICANN::SynapseWeight(dw), sc.getDigitalWeight(aw));
	}
	ASSERT_TRUE(sc.check_monotonic_increasing());
	sc.reset(0,Polynomial::create({0,-1}));
	ASSERT_EQ(sc.getAnalogWeight(HICANN::SynapseWeight(1)), -1);
	ASSERT_FALSE(sc.check_monotonic_increasing());
}

TEST(Calibtic, GmaxConfig)
{
	using namespace HMF;
	GmaxConfig gc1({0,11});
	GmaxConfig gc2({0,12});
	GmaxConfig gc3({1,11});
	GmaxConfig gc4({1,12});
	GmaxConfig gc5({0,11});
	ASSERT_TRUE(gc1 < gc2);
	ASSERT_TRUE(gc1 < gc3);
	ASSERT_TRUE(gc2 < gc3);
	ASSERT_TRUE(gc2 < gc4);
	ASSERT_EQ(gc1,gc5);
	ASSERT_FALSE(gc1<gc5);
	ASSERT_FALSE(gc5<gc1);
	ASSERT_FALSE(gc1 == gc2);
	ASSERT_FALSE(gc2 < gc1);
	ASSERT_EQ(gc1, GmaxConfig::Default());
}

TEST(Calibtic, SynapseRowCalibration)
{
	using namespace HMF;
	SynapseRowCalibration src;
	src.setDefaults();
	ASSERT_EQ(1,src.size());
	ASSERT_TRUE(src.exists(GmaxConfig(0,11)));
	ASSERT_FALSE(src.exists(GmaxConfig(1,11)));
	ASSERT_EQ(1,src.erase(GmaxConfig(0,11)));
	ASSERT_EQ(0,src.erase(GmaxConfig(0,11)));
	ASSERT_EQ(0,src.size());

	src.setDefaults();
	ASSERT_EQ(1,src.size());
	src.clear();
	ASSERT_EQ(0,src.size());
}

TEST(SynapseRowCalibration, FindBestConfig)
{
	using namespace HMF;
	SynapseRowCalibration src;
	shared_ptr<SynapseCalibration> s1(new SynapseCalibration);
	shared_ptr<SynapseCalibration> s2(new SynapseCalibration);
	s1->reset(0,Polynomial::create({0,1})); // y=x
	s2->reset(0,Polynomial::create({1,1})); // y=1+x
	GmaxConfig c1(0,1);
	GmaxConfig c2(1,1);
	GmaxConfig c_default = GmaxConfig::Default();

	src.insert(c1,s1);
	src.insert(c2,s2);
	double max_required_weight_1 = 14.1;
	double max_required_weight_2 = 15.6;
	double max_required_weight_3 = 16.5;

	ASSERT_EQ(c1, src.findBestGmaxConfig(max_required_weight_1));
	ASSERT_EQ(c2, src.findBestGmaxConfig(max_required_weight_2));
	ASSERT_EQ(c2, src.findBestGmaxConfig(max_required_weight_3));

	src.clear();
	ASSERT_EQ(c_default, src.findBestGmaxConfig(1.));
	src.clear();
	src.setDefaults();
	ASSERT_EQ(c_default, src.findBestGmaxConfig(1.));
}

TEST(Calibtic, SynapseRowCollection)
{
	using namespace HMF;
	SynapseRowCollection srco;
	srco.setDefaults();
	ASSERT_EQ(448, srco.size());
}

TEST(Calibtic, L1CrossbarCalibration)
{
	HMF::L1CrossbarCalibration l1;
	l1.setDefaults();
	ASSERT_EQ(1, l1.getMaxSwitchesPerRow());
	ASSERT_EQ(1, l1.getMaxSwitchesPerColumn());

	l1.setMaxSwitchesPerRow(3);
	l1.setMaxSwitchesPerColumn(5);

	ASSERT_EQ(3, l1.getMaxSwitchesPerRow());
	ASSERT_EQ(5, l1.getMaxSwitchesPerColumn());
}

TYPED_TEST(BasicTest, L1CrossbarCalibration)
{
	MetaData md;
	HMF::L1CrossbarCalibration l1;
	l1.setMaxSwitchesPerRow(13);
	l1.setMaxSwitchesPerColumn(37);
	TestFixture::backend->store("fisch", md, l1);

	for (size_t ii = 0; ii < 2; ii++) {
		HMF::L1CrossbarCalibration ll;
		TestFixture::backend->load("fisch", md, ll);
		ASSERT_EQ(13, ll.getMaxSwitchesPerRow());
		ASSERT_EQ(37, ll.getMaxSwitchesPerColumn());
	}
}

TEST(Calibtic, L1CrossbarCollection)
{
	using namespace HMF;
	L1CrossbarCollection l1co;
	l1co.setDefaults();
	ASSERT_EQ(1, l1co.getMaxSwitchesPerRow(HMF::Coordinate::VLineOnHICANN(42)));
	ASSERT_EQ(1, l1co.getMaxSwitchesPerRow(HMF::Coordinate::HLineOnHICANN(42)));

	l1co.setMaxSwitchesPerRow(HMF::Coordinate::VLineOnHICANN(42), 4);

	ASSERT_EQ(4, l1co.getMaxSwitchesPerRow(HMF::Coordinate::VLineOnHICANN(42)));
	ASSERT_EQ(1, l1co.getMaxSwitchesPerRow(HMF::Coordinate::HLineOnHICANN(42)));
}

TEST(calibtic, L1CrossbarCollection)
{
	using namespace HMF;
	L1CrossbarCollection l1co;
	l1co.setDefaults();

	for( size_t i : Coordinate::iter_all<Coordinate::VLineOnHICANN>() ){
		l1co.setMaxSwitchesPerRow(Coordinate::VLineOnHICANN(i), 4);
	}
	for( size_t i : Coordinate::iter_all<Coordinate::HLineOnHICANN>() ){
		ASSERT_EQ(1, l1co.getMaxSwitchesPerRow(Coordinate::HLineOnHICANN(i)));
	}

}

TYPED_TEST(BasicTest, L1CrossbarCollection)
{
	using namespace HMF;
	L1CrossbarCollection l1co;
	l1co.setDefaults();
	ASSERT_EQ(1, l1co.getMaxSwitchesPerRow(HMF::Coordinate::VLineOnHICANN(42)));
	ASSERT_EQ(1, l1co.getMaxSwitchesPerRow(HMF::Coordinate::HLineOnHICANN(42)));

	l1co.setMaxSwitchesPerRow(HMF::Coordinate::VLineOnHICANN(42), 4);

	ASSERT_EQ(4, l1co.getMaxSwitchesPerRow(HMF::Coordinate::VLineOnHICANN(42)));
	ASSERT_EQ(1, l1co.getMaxSwitchesPerRow(HMF::Coordinate::HLineOnHICANN(42)));

	// test serialisation
	MetaData md;
	TestFixture::backend->store("fisch", md, l1co);

	for (size_t ii = 0; ii < 2; ii++) {
		L1CrossbarCollection l1co_load;
		TestFixture::backend->load("fisch", md, l1co_load);

		ASSERT_EQ(4, l1co_load.getMaxSwitchesPerRow(HMF::Coordinate::VLineOnHICANN(42)));
		ASSERT_EQ(1, l1co_load.getMaxSwitchesPerRow(HMF::Coordinate::HLineOnHICANN(42)));
	}
}

TEST(Calibtic, SynapseChainLengthCollection)
{
	HMF::SynapseChainLengthCollection ch;
	ch.setDefaults();
	ASSERT_EQ(3, ch.getMaxChainLength(HMF::Coordinate::VLineOnHICANN(42)));

	ch.setMaxChainLength(HMF::Coordinate::VLineOnHICANN(42), 1);

	ASSERT_EQ(1, ch.getMaxChainLength(HMF::Coordinate::VLineOnHICANN(42)));
}

TYPED_TEST(BasicTest, SynapseChainLengthCollection)
{
	MetaData md;
	HMF::SynapseChainLengthCollection ch;
	ch.setMaxChainLength(HMF::Coordinate::VLineOnHICANN(42), 42);
	TestFixture::backend->store("fisch", md, ch);

	for (size_t ii = 0; ii < 2; ii++) {
		HMF::SynapseChainLengthCollection cc;
		TestFixture::backend->load("fisch", md, cc);
		ASSERT_EQ(42, cc.getMaxChainLength(HMF::Coordinate::VLineOnHICANN(42)));
	}
}

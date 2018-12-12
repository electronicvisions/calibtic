#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <numeric>
#include <cmath>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include "calibtic/Collection.h"
#include "calibtic/Calibration.h"
#include "calibtic/backend/Library.h"
#include "calibtic/backend/Backend.h"
#include "calibtic/trafo/Transformation.h"
#include "calibtic/trafo/Polynomial.h"
#include "calibtic/trafo/SumOfTrafos.h"
#include "calibtic/trafo/PowerOfTrafo.h"
#include "calibtic/trafo/OneOverPolynomial.h"
#include "calibtic/trafo/NegativePowersPolynomial.h"
#include "calibtic/trafo/Constant.h"

#include "calibtic/HMF/NeuronCalibration.h"
#include "calibtic/HMF/NeuronCollection.h"

#include <calibtic/backends/export.ipp>

using namespace calibtic;
using namespace calibtic::backend;
using namespace calibtic::trafo;
using namespace boost::serialization;
using boost::shared_ptr;

// TODO: Make this a typed test for all calibtic types to test general
// serializability.


TEST(Serialization, Transformation)
{
	boost::shared_ptr<Transformation> trafo0;
	boost::shared_ptr<Transformation> trafo1;

	std::stringstream stream;
	{
		boost::archive::xml_oarchive oa(stream);
		oa << make_nvp("trafo", trafo0);
	}

	{
		boost::archive::xml_iarchive ia(stream);
		ia >> make_nvp("trafo", trafo1);
	}
}


TEST(Serialization, Constant)
{

	boost::shared_ptr<Transformation> trafo0(new Constant(10));
	boost::shared_ptr<Transformation> trafo1;

	std::stringstream stream;
	{
		boost::archive::xml_oarchive oa(stream);
		oa << make_nvp("trafo", trafo0);
	}

	{
		boost::archive::xml_iarchive ia(stream);
		ia >> make_nvp("trafo", trafo1);
	}

	ASSERT_EQ(*trafo0, *trafo1);
}


TEST(Serialization, Polynomial)
{

	std::vector<double> coeff = { 1,2,3,4,5 };

	boost::shared_ptr<Transformation> trafo0(new Polynomial(coeff));
	boost::shared_ptr<Transformation> trafo1;

	std::stringstream stream;
	{
		boost::archive::xml_oarchive oa(stream);
		oa << make_nvp("trafo", trafo0);
	}


	{
		boost::archive::xml_iarchive ia(stream);
		ia >> make_nvp("trafo", trafo1);
	}

	ASSERT_EQ(*trafo0, *trafo1);
}

TEST(Serialization, OneOverPolynomial)
{

	std::vector<double> coeff = { 4, 3, 2, 1 };

	boost::shared_ptr<Transformation> trafo0(new OneOverPolynomial(coeff));
	boost::shared_ptr<Transformation> trafo1;

	std::stringstream stream;
	{
		boost::archive::xml_oarchive oa(stream);
		oa << make_nvp("trafo", trafo0);
	}


	{
		boost::archive::xml_iarchive ia(stream);
		ia >> make_nvp("trafo", trafo1);
	}

	ASSERT_EQ(*trafo0, *trafo1);
}

TEST(Serialization, InvQuadraticPol)
{

	InvQuadraticPol::data_type coeff = {{ 4, 3, 2, 1 }};

	boost::shared_ptr<Transformation> trafo0(new InvQuadraticPol(coeff));
	boost::shared_ptr<Transformation> trafo1;

	std::stringstream stream;
	{
		boost::archive::xml_oarchive oa(stream);
		oa << make_nvp("trafo", trafo0);
	}

	{
		boost::archive::xml_iarchive ia(stream);
		ia >> make_nvp("trafo", trafo1);
	}

	ASSERT_EQ(*trafo0, *trafo1);
}

TEST(Serialization, NegativePowersPolynomial)
{

	std::vector<double> coeff = { 4, 3, 2, 1 };

	boost::shared_ptr<Transformation> trafo0(new NegativePowersPolynomial(coeff));
	boost::shared_ptr<Transformation> trafo1;

	std::stringstream stream;
	{
		boost::archive::xml_oarchive oa(stream);
		oa << make_nvp("trafo", trafo0);
	}


	{
		boost::archive::xml_iarchive ia(stream);
		ia >> make_nvp("trafo", trafo1);
	}

	ASSERT_EQ(*trafo0, *trafo1);
}

TEST(Serialization, SumOfTrafos)
{

	SumOfTrafos::trafo_list trafos = {boost::shared_ptr<Transformation>(new Polynomial()),
	                                  boost::shared_ptr<Transformation>(new Constant())};

	boost::shared_ptr<Transformation> trafo0(new SumOfTrafos(trafos));
	boost::shared_ptr<Transformation> trafo1;

	std::stringstream stream;
	{
		boost::archive::xml_oarchive oa(stream);
		oa << make_nvp("trafo", trafo0);
	}


	{
		boost::archive::xml_iarchive ia(stream);
		ia >> make_nvp("trafo", trafo1);
	}

	ASSERT_EQ(*trafo0, *trafo1);
}

TEST(EQ, SumOfTrafos)
{
	SumOfTrafos::trafo_list trafos_const = {
	    boost::shared_ptr<Transformation>(new Constant(5)),
	    boost::shared_ptr<Transformation>(new Constant(7))};

	SumOfTrafos o1(trafos_const);

	SumOfTrafos::trafo_list trafos_mix = {
	    boost::shared_ptr<Transformation>(new NegativePowersPolynomial({1., 5.})),
	    boost::shared_ptr<Transformation>(new Constant(7))};

	SumOfTrafos o2(trafos_mix);

	SumOfTrafos o3(trafos_const);

	ASSERT_FALSE(o1 == o2);
	ASSERT_TRUE(o1 == o3);
}

TEST(Serialization, PowerOfTrafo)
{

	boost::shared_ptr<Transformation> trafo0(new PowerOfTrafo(3, boost::shared_ptr<Transformation>(new Constant(5))));
	boost::shared_ptr<Transformation> trafo1;

	std::stringstream stream;
	{
		boost::archive::xml_oarchive oa(stream);
		oa << make_nvp("trafo", trafo0);
	}


	{
		boost::archive::xml_iarchive ia(stream);
		ia >> make_nvp("trafo", trafo1);
	}

	ASSERT_EQ(*trafo0, *trafo1);
}

TEST(EQ, PowerOfTrafo)
{

	PowerOfTrafo o1(3, boost::shared_ptr<Transformation>(new Constant(5)));
	PowerOfTrafo o2(3, boost::shared_ptr<Transformation>(new Constant(6)));
	PowerOfTrafo o3(3, boost::shared_ptr<Transformation>(new Constant(5)));
	PowerOfTrafo o4(4, boost::shared_ptr<Transformation>(new Constant(5)));
	PowerOfTrafo o5(4, boost::shared_ptr<Transformation>());
	PowerOfTrafo o6(5, boost::shared_ptr<Transformation>());
	PowerOfTrafo o7(4, boost::shared_ptr<Transformation>());

	ASSERT_FALSE(o1 == o2);
	ASSERT_TRUE(o1 == o3);
	ASSERT_FALSE(o1 == o4);
	ASSERT_FALSE(o3 == o4);
	ASSERT_FALSE(o4 == o5);
	ASSERT_FALSE(o5 == o6);
	ASSERT_TRUE(o5 == o7);

}

TEST(Domain, Polynomial)
{
	std::vector<double> coeff = {0,0,1}; // x^2
	// polynomial is defined on domain [0.0, 3.0]
	boost::shared_ptr<Polynomial> poly0(new Polynomial(coeff, 0.0, 3.0));
	boost::shared_ptr<Polynomial> poly1(new Polynomial(coeff, 7.3, 21.8));

	// test values inside and outside of domain
	ASSERT_TRUE(poly0->in_domain(0.0, poly0->getDomain()));
	ASSERT_TRUE(poly0->in_domain(0.5, poly0->getDomain()));
	ASSERT_TRUE(poly0->in_domain(2.9, poly0->getDomain()));
	ASSERT_TRUE(poly0->in_domain(3.0, poly0->getDomain()));
	ASSERT_FALSE(poly0->in_domain(3.1, poly0->getDomain()));
	ASSERT_FALSE(poly0->in_domain(-0.1, poly0->getDomain()));

	// test operator==
	ASSERT_FALSE(poly0 == poly1);

	// is polynomial x^2 and
	// are values inside domain allowed?
	ASSERT_EQ(poly0->apply(2), 4);
	ASSERT_EQ(poly0->apply(3), 9);

	// are values outside domain forbidden?
	ASSERT_THROW(poly0->apply(4, Transformation::THROW), OutsideDomainException);

	// no exception thrown for the IGNORE option
	ASSERT_NO_THROW(poly0->apply(4, Transformation::IGNORE));
	ASSERT_EQ(poly0->apply(8, Transformation::IGNORE), poly1->apply(8, Transformation::IGNORE));

	// Test the clipping
	ASSERT_NO_THROW(poly0->apply(4, Transformation::CLIP));
	ASSERT_EQ(poly0->apply(4), poly0->apply(3.0));
	ASSERT_EQ(poly0->apply(-1), poly0->apply(0.0));
}

TEST(Domain, Lookup)
{
	std::vector<double> lookup_data1(20);
	for (size_t ii = 0; ii < lookup_data1.size(); ++ii) {
		lookup_data1[ii] = exp(double(ii)/20.0);
	}

	std::vector<double> lookup_data2(45);
	for (size_t ii = 0; ii < lookup_data2.size(); ++ii) {
		lookup_data2[ii] = 1.0/double(ii + 1.0);
	}

	Lookup lookup1(lookup_data1, 5);
	Lookup lookup2(lookup_data2);

	EXPECT_EQ(lookup1.apply(1.01), 5.0);
	EXPECT_EQ(lookup1.apply(1.5), 13.0);
	EXPECT_EQ(lookup1.apply(2.5), 23.0);
	ASSERT_NO_THROW(lookup1.apply(1.0, Transformation::THROW));
	ASSERT_THROW(lookup1.apply(exp(1.0), Transformation::THROW), OutsideDomainException);
	ASSERT_THROW(lookup1.apply(0.99, Transformation::THROW), OutsideDomainException);

	// 1.0 0.5 0.33 0.25 0.20 ...
	EXPECT_EQ(lookup2.apply(0.4), 1.0);
	EXPECT_EQ(lookup2.apply(0.2), 4.0);
	EXPECT_EQ(lookup2.apply(0.0225), 43.0);
	ASSERT_THROW(lookup2.apply(1.1, Transformation::THROW), OutsideDomainException);
	// ASSERT_THROW(lookup2.apply(1.0), OutsideDomainException);
	ASSERT_THROW(lookup2.apply(0.0, Transformation::THROW), OutsideDomainException);
}

TEST(Serialization, Lookup)
{


	std::vector<double> lookup_data1(20);
	for (size_t ii = 0; ii < lookup_data1.size(); ++ii) {
		lookup_data1[ii] = exp(double(ii)/20.0);
	}

	boost::shared_ptr<Transformation> trafo0(new Lookup(lookup_data1, 5));
	boost::shared_ptr<Transformation> trafo1;

	std::stringstream stream;
	{
		boost::archive::xml_oarchive oa(stream);
		oa << make_nvp("trafo", trafo0);
	}


	{
		boost::archive::xml_iarchive ia(stream);
		ia >> make_nvp("trafo", trafo1);
	}

	ASSERT_EQ(*trafo0, *trafo1);

}

static void sort(std::vector<std::complex<float_type>> & v)
{
	typedef std::complex<float_type> t;
	std::sort(v.begin(), v.end(), [](const t & a, const t & b) {
			return (a.real() == b.real()) ? (a.imag() < b.imag()) : (a.real() < b.real());});
}

// Macro shows the right position of the error
#define CHECK_FIND_ROOTS(VALUE, MIN, MAX)						   \
	boost::shared_ptr<Polynomial> poly(new Polynomial(coeff, MIN, MAX));	\
	auto result = poly->find_roots(VALUE); \
	ASSERT_EQ(expected.size(), result.size()); \
	sort(result); \
	for (size_t ii = 0; ii < expected.size(); ++ii) \
	{ \
		EXPECT_NEAR(std::abs(expected[ii] - result[ii]), 0.0, 1e-11) << \
			"Result " << ii << " differs: found " << result[ii] << \
			" expected " << expected[ii]; \
	} \

TEST(Polynomial, FindRoots)
{
	typedef std::vector<std::complex<float_type>> result_type;
	{
		result_type expected({{0.0, 0.0}, {0.0, 0.0}});
		std::vector<double> coeff = {0,0,5}; // 5 * x^2
		CHECK_FIND_ROOTS(0.0, 0, 100);
	}

	{
		result_type expected({{-1.0, 0.0}, {1.0, 0.0}});
		std::vector<double> coeff = {0,0,5}; // 5 * x^2
		CHECK_FIND_ROOTS(5.0, 0, 100);
	}

	{
		result_type expected({{0.0, 0.0}, {0.0, 0.0}, {2.0, 0.0}});
		std::vector<double> coeff = {30, 0, -10, 5}; // 5 * x^3 - 10 * x^2 + 30
		CHECK_FIND_ROOTS(30.0, 0, 100);
	}

	{
		result_type expected({
				{-0.38883190541500517, -1.4173561209752508},
				{-0.38883190541500517,  1.4173561209752508},
				{2.7776638108300116, 0.0}});
		std::vector<double> coeff = {30, 0, -10, 5}; // 5 * x^3 - 10 * x^2 + 30
		CHECK_FIND_ROOTS(60.0, 0, 100);
	}
}

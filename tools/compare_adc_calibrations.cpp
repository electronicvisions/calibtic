#include <algorithm>
#include <chrono>
#include <iostream>
#include <functional>
#include <random>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/mean.hpp>

#include "calibtic/HMF/ADC/ADCCalibration.h"
#include "calibtic/HMF/ADC/QuadraticADCCalibration.h"
#include "calibtic/trafo/Polynomial.h"

using namespace HMF::ADC;

int main()
{
	const size_t repeat = 10;
	std::vector<uint16_t> data(2000000);
	std::generate(data.begin(), data.end(), std::bind(
		std::uniform_int_distribution<int>(0, 4095),
		std::mt19937_64()));

	ADCCalibration::key_type channel;
	boost::shared_ptr<ADCCalibration> regular(new ADCCalibration());
	*regular = ADCCalibration::getDefaultCalibration();
	*(regular->at(0)) = calibtic::trafo::Polynomial{{
		2.01354, -0.000661921, 5.55852e-09}};
	boost::shared_ptr<QuadraticADCCalibration> quadratic(
		ADCCalibration::convertToQuadraticADCCalibration(*regular));

	{
		using namespace boost::accumulators;
		accumulator_set<double, stats<tag::mean, tag::min>> acc;

		std::vector<float> result;
		std::cout << "Testing ADCCalibration: " << std::endl;
		for (size_t ii = 0; ii < repeat; ++ii)
		{
			auto t1=std::chrono::high_resolution_clock::now();
			result = regular->apply(channel, data);
			auto t2 = std::chrono::high_resolution_clock::now();
			acc(std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count());
		}
		std::cout << "    fastest: " << min(acc) << "ms\n";
		std::cout << "    average: " << mean(acc) << "ms\n";
		std::cout << "    first values:";
		for (size_t ii = 0; ii < 5; ++ii) {
			std::cout << " " << result[ii];
		}
		std::cout << " ..." << std::endl;
	}

	{
		std::vector<float> result;
		using namespace boost::accumulators;
		accumulator_set<double, stats<tag::mean, tag::min>> acc;

		std::cout << "Testing QuadraticADCCalibration: " << std::endl;
		for (size_t ii = 0; ii < repeat; ++ii)
		{
			auto t1=std::chrono::high_resolution_clock::now();
			result = quadratic->apply(channel, data);
			auto t2 = std::chrono::high_resolution_clock::now();
			acc(std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count());
		}
		std::cout << "    fastest: " << min(acc) << "ms\n";
		std::cout << "    average: " << mean(acc) << "ms\n";
		std::cout << "    first values:";
		for (size_t ii = 0; ii < 5; ++ii) {
			std::cout << " " << result[ii];
		}
		std::cout << " ..." << std::endl;
	}
}


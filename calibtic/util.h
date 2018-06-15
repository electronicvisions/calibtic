#pragma once
#include <random>

namespace calibtic {

template<typename OutputType, typename InputType>
OutputType stochastic_round(InputType input)
{
	static_assert(std::is_floating_point<InputType>::value,
				  "input must be floating point");
	static std::mt19937 gen;
	return std::generate_canonical<InputType>(gen) < input-static_cast<OutputType>(input) ?
		++input : input;
}


template<typename T>
T clip(T val, T min, T max)
{
	return val > max ?
		max : (val < min ? min : val);
}

} // calibtic

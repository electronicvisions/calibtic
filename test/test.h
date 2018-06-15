#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include <random>
#include <type_traits>

#include "calibtic/backend/Library.h"
#include "calibtic/backend/Backend.h"

#include "logging_ctrl.h"

template<typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
random(T const max = std::numeric_limits<T>::max(),
	   T const min = std::numeric_limits<T>::lowest())
{
	typedef typename std::conditional<
			std::is_integral<T>::value,
			std::uniform_int_distribution<T>,
			std::uniform_real_distribution<T>
		>::type type;

	static std::random_device rng;
	type dist(min, max);
	return dist(rng);
}

#define REPEAT(N) for(size_t __ii=0; __ii<N; __ii++)

boost::shared_ptr<calibtic::backend::Backend> init_my_backend(std::string const& fname);

template <typename T>
class TestWithBackend : public ::testing::Test {};
struct XMLBackend {};

template<>
class TestWithBackend<XMLBackend> : public ::testing::Test
{
public:
	static void SetUpTestCase()
	{
		using namespace boost::filesystem;

		backend = init_my_backend("libcalibtic_xml.so");
		ASSERT_TRUE(static_cast<bool>(backend));

		backendPath = boost::filesystem::unique_path();
		boost::filesystem::create_directories(backendPath);
		backend->config("path", backendPath.native());
		backend->init();
	}

	static void TearDownTestCase()
	{
		boost::filesystem::remove_all(backendPath);
		backendPath.clear();
	}

	static boost::shared_ptr<calibtic::backend::Backend> backend;
	static boost::filesystem::path backendPath;
};
typedef ::testing::Types<XMLBackend> BackendTypes;

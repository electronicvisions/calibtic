#include <stdexcept>
#include "test/test.h"

using namespace calibtic::backend;

boost::shared_ptr<Backend> TestWithBackend<XMLBackend>::backend;
boost::filesystem::path TestWithBackend<XMLBackend>::backendPath;

boost::shared_ptr<Backend> init_my_backend(std::string const& fname)
{
	auto lib = loadLibrary(fname);
	boost::shared_ptr<Backend> backend = loadBackend(lib);

	if (!backend) {
		std::stringstream s;
		s << "unable to load: " << fname;
		throw std::runtime_error(s.str());
	}

	return backend;
}

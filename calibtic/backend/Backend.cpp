#include "calibtic/backend/Backend.h"
#include <sstream>
#include <stdexcept>
#include <dlfcn.h>

#include "calibtic/backend/Library.h"
#include "calibtic/backend/BackendDeleter.h"

namespace calibtic {
namespace backend {

void Backend::config(
	std::string const& key,
	std::string const& val)
{
	config_(key, config_value_t(val));
}

void Backend::config(
	std::string const& key,
	int const val)
{
	config_(key, config_value_t(val));
}

// for Python, boost wrapping might call double
void Backend::config_int(
	std::string const& key,
	int const val)
{
	config_(key, config_value_t(val));
}

void Backend::config(
	std::string const& key,
	double const val)
{
	config_(key, config_value_t(val));
}

void Backend::config_(
	std::string const& key,
	config_value_t const& val)
{
	mConfig[key] = val;
	if (val.empty()) {
		mConfig.erase(key);
	}
}

bool Backend::exists(std::string const& key) const
{
	auto it = mConfig.find(key);
	return (it != mConfig.end());
}


boost::shared_ptr<Backend>
loadBackend(boost::shared_ptr<Library> lib)
{
	typedef Backend* create_t();
	const char symbol[] = {"createBackend"};

	create_t* create = nullptr;
	*(void**)(&create) = dlsym(lib->get(), symbol);

	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		std::ostringstream sstr;
		sstr << "Cannot load symbol " << symbol << ": "
			<< dlsym_error << std::endl;
		throw std::runtime_error(sstr.str());
	}

	Backend* backend = create();

	return boost::shared_ptr<Backend>(backend, BackendDeleter(lib));
}

} // backend
} // calibtic

#include "calibtic/backend/Library.h"

#include <stdexcept>
#include <sstream>
#include <dlfcn.h>
#include <cstdlib>

namespace calibtic {
namespace backend {

Library::Library(std::string const& name, bool const& set_lang) :
	ptr(nullptr)
{
	if (set_lang) {
		// workaround for issue #1468
		setenv("LANG", "C", 1);
		setenv("LANGUAGE", "C", 1);
		setenv("LC_ALL", "C", 1);
	}
	ptr = dlopen(name.c_str(), RTLD_LAZY);
	if (!ptr) {
		std::ostringstream sstr;
		sstr << "Cannot load library: " << dlerror() << std::endl;
		throw std::runtime_error(sstr.str());
	}

	// reset errors
	dlerror();
}

Library::~Library()
{
	dlclose(ptr);
}

void* Library::get()
{
	return ptr;
}

void const* Library::get() const
{
	return ptr;
}


boost::shared_ptr<Library>
loadLibrary(std::string const& name)
{
	return boost::shared_ptr<Library>(new Library(name));
}

} // backend
} // calibtic

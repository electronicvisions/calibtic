#include <dlfcn.h>
#include <sstream>
#include <stdexcept>

#include "calibtic/backend/BackendDeleter.h"
#include "calibtic/backend/Backend.h"

namespace calibtic {
namespace backend {

BackendDeleter::BackendDeleter(boost::shared_ptr<Library> lib) :
	lib(lib) {}

void BackendDeleter::operator () (Backend* b) const
{
	destroy_backend(lib->get(), b);
}

void BackendDeleter::destroy_backend(void* library, Backend* backend) const
{
	const char symbol[] = {"destroyBackend"};

	void* pobj = dlsym(library, symbol);
	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		std::ostringstream sstr;
		sstr << "Cannot load symbol " << symbol << ": "
			<< dlsym_error << std::endl;
		throw std::runtime_error(sstr.str());
	}

	// See comment in redman::loadBackend().
	void (*destroy)(Backend*);
	*reinterpret_cast<void **>(&destroy) = pobj;

	// destroy the class
	destroy(backend);
}

} // backend
} // calibtic

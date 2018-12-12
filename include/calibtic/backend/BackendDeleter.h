#pragma once

#include <boost/shared_ptr.hpp>

#include "calibtic/backend/Library.h"

namespace calibtic {
namespace backend {

// fwd decl;
class Backend;

class BackendDeleter
{
public:
	BackendDeleter(boost::shared_ptr<Library> lib);

	void operator () (Backend* b) const;

private:
	void destroy_backend(
		void* library,
		Backend* backend) const;

	boost::shared_ptr<Library> lib;
};

} // backend
} // calibtic

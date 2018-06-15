#include "calibtic/Base.h"

//BOOST_CLASS_EXPORT_IMPLEMENT(calibtic::Base)

namespace calibtic {

Base::~Base() {}

std::ostream& operator<< (std::ostream& os, Base const& t)
{
	return t.operator<<(os);
}

} // calibtic

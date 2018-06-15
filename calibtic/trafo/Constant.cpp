#include "calibtic/trafo/Constant.h"

namespace calibtic {
namespace trafo {

Constant::Constant(float_type const& val) :
	mData(val) {}

Constant::~Constant() {}

float_type
Constant::apply(float_type const&, OutsideDomainBehavior /*outside_domain_behavior*/) const
{
	return mData;
}

float_type
Constant::reverseApply(float_type const& /*in*/, Transformation::OutsideDomainBehavior /*outside_domain_behavior*/) const
{
	throw std::runtime_error("Constants cannot be reversed.");
}

bool
Constant::operator== (Transformation const& rhs) const
{
	Constant const* _rhs = dynamic_cast<Constant const*>(&rhs);
	if (!_rhs) {
		return false;
	}

	return (mData == _rhs->mData);
}

std::ostream&
Constant::operator<< (std::ostream& os) const
{
	os << "Constant: " << mData;
	return os;
}

float_type
Constant::getData() const
{
	return mData;
}

float_type&
Constant::getData()
{
	return mData;
}

boost::shared_ptr<Constant>
Constant::create(float_type val)
{
	    return boost::shared_ptr<Constant>(new Constant(val));
}

} // trafo
} // calibtic

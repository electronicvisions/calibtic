#include "calibtic/trafo/OneOverPolynomial.h"
#include <boost/icl/interval_bounds.hpp>
#include <cmath>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_poly.h>


namespace calibtic {
namespace trafo {

OneOverPolynomial::OneOverPolynomial(data_type const& coeff, float_type const& min, float_type const& max) :
	mPolynomial(coeff, min, max)
{
	setDomain(min,max);
}

OneOverPolynomial::~OneOverPolynomial() {}

float_type
OneOverPolynomial::apply(float_type const& in, OutsideDomainBehavior outside_domain_behavior) const
{

	float_type val = respectDomain(in, outside_domain_behavior);

	float_type r = mPolynomial.apply(val, outside_domain_behavior);

	return 1/r;
}

float_type
OneOverPolynomial::reverseApply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior) const
{
	if (in == 0) {
		throw std::runtime_error("reverseApply(0) not possible.");
	}

	float_type const val = respectReverseDomain(in, outside_domain_behavior);

	return mPolynomial.reverseApply(1/val, outside_domain_behavior);
}

OneOverPolynomial::data_type const&
OneOverPolynomial::getData() const
{
	return mPolynomial.getData();
}

OneOverPolynomial::data_type&
OneOverPolynomial::getData()
{
	return mPolynomial.getData();
}

bool
OneOverPolynomial::operator== (Transformation const& rhs) const
{
	OneOverPolynomial const* _rhs = dynamic_cast<OneOverPolynomial const*>(&rhs);
	if (!_rhs) {
		return false;
	}

	return (getDomain() == _rhs->getDomain()
		    && mPolynomial == _rhs->mPolynomial);
}

std::ostream&
OneOverPolynomial::operator<< (std::ostream& os) const
{
	os << "OneOverPolynomial: ";
	for (int index = 0, size = mPolynomial.getData().size(); index < size; ++index) {
		auto const& val = mPolynomial.getData()[index];
		if (index == 0)
		{
			os << "1 / (" << val;
		}
		else if (index == size-1)
		{
			os << " + " << val << "*x^" << index << ")";
		}
		else
		{
			os << " + " << val << "*x^" << index;
		}
	}
	os << ", domain: " << getDomain();
	return os;
}

boost::shared_ptr<OneOverPolynomial>
OneOverPolynomial::create(data_type const& coeff,
		float_type const& min, float_type const& max)
{
	return boost::shared_ptr<OneOverPolynomial>(new OneOverPolynomial(coeff, min, max));
}

} // trafo
} // calibtic

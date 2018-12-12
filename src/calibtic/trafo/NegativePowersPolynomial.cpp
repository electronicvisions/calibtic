#include "calibtic/trafo/NegativePowersPolynomial.h"
#include <boost/icl/interval_bounds.hpp>
#include <cmath>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_poly.h>


namespace calibtic {
namespace trafo {

NegativePowersPolynomial::NegativePowersPolynomial(data_type const& coeff, float_type const& min, float_type const& max) :
	Polynomial(coeff, min, max)
{
	setDomain(min,max);
}

NegativePowersPolynomial::~NegativePowersPolynomial() {}

float_type
NegativePowersPolynomial::apply(float_type const& in, OutsideDomainBehavior outside_domain_behavior) const
{

	if (in == 0 && mData.size() > 1) { // only check if polynomial is not a constant
		throw std::runtime_error("Division by zero.");
	}

	float_type val = respectDomain(in, outside_domain_behavior);

	float_type r = 0.;
	unsigned exp = 0;
	for (float_type const& d: Polynomial::getData())
	{
		r += d * 1/pow(val, exp++);
	}
	return r;
}

float_type
NegativePowersPolynomial::reverseApply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior) const
{
	float_type result = Polynomial::reverseApply(in, outside_domain_behavior);
	return result;
}

bool
NegativePowersPolynomial::operator== (Transformation const& rhs) const
{
	NegativePowersPolynomial const* _rhs = dynamic_cast<NegativePowersPolynomial const*>(&rhs);
	if (!_rhs) {
		return false;
	}

	return (getData() == _rhs->getData() && getDomain() == _rhs->getDomain());
}

std::ostream&
NegativePowersPolynomial::operator<< (std::ostream& os) const
{
	os << "NegativePowersPolynomial: ";
	for (int index = 0, size = Polynomial::getData().size(); index < size; ++index) {
		auto const& val = Polynomial::getData()[index];
		if (index == 0)
		{
			os << val;
		}
		else
		{
			os << " + " << val << "*x^-" << index;
		}
	}
	os << ", domain: " << getDomain();
	return os;
}

Polynomial::data_type
NegativePowersPolynomial::find_real_roots(float_type const val, bool in_domain) const
{
	data_type result;
	for (auto root : find_roots(val))
	{
		if (root.imag() == 0.0)
		{
			if (!in_domain || Polynomial::in_domain(1/root.real(), getDomain()))
			{
				result.push_back(1/root.real());
			}
		}
	}
	return result;
}

boost::shared_ptr<NegativePowersPolynomial>
NegativePowersPolynomial::create(data_type const& coeff,
		float_type const& min, float_type const& max)
{
	return boost::shared_ptr<NegativePowersPolynomial>(new NegativePowersPolynomial(coeff, min, max));
}

} // trafo
} // calibtic

#include "calibtic/trafo/Polynomial.h"
#include <boost/icl/interval_bounds.hpp>
#include <cmath>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_poly.h>

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr _log = log4cxx::Logger::getLogger("Calibtic");

namespace calibtic {
namespace trafo {

Polynomial::Polynomial(data_type const& coeff,
					   float_type const& min,
                       float_type const& max)
    : mData(coeff) {
	setDomain(min, max);
}

Polynomial::~Polynomial() {}

float_type
Polynomial::apply(float_type const& in, OutsideDomainBehavior outside_domain_behavior) const
{
	float_type val = respectDomain(in, outside_domain_behavior);

	float_type r = 0.;
	unsigned exp = 0;
	for (float_type const& d: mData)
	{
		r += d * pow(val, exp++);
	}
	return r;
}

float_type
Polynomial::reverseApply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior) const
{
	if (mData.size() < 2) {
		throw std::runtime_error("Reverse transformation of constant not possible");
	}

	float_type const val = Polynomial::respectReverseDomain(in, outside_domain_behavior);

	data_type real_roots_in_domain = find_real_roots(val, true);
	if (real_roots_in_domain.size() != 1) {
		throw OutsideDomainException("No solutions or more than one solution in domain found.");
	}
	return real_roots_in_domain[0];
}


std::vector<std::complex<float_type>>
Polynomial::find_roots(const float_type val) const
{
	const size_t dim = mData.size();

	std::vector<std::complex<float_type>> result;

	if (dim < 1) {
		return result;
	}

	Polynomial::data_type data(dim);
	std::copy(mData.begin(), mData.end(), data.begin());
	data[0] -= val;

	Polynomial::data_type tmp((dim - 1) * 2);
	std::unique_ptr<gsl_poly_complex_workspace,
		void (*)(gsl_poly_complex_workspace*)> w(
			gsl_poly_complex_workspace_alloc(dim),
			&gsl_poly_complex_workspace_free);
	if (gsl_poly_complex_solve(data.data(), dim, w.get(), tmp.data())
		!= GSL_SUCCESS)
	{
		throw std::runtime_error("Search for roots didn't converge");
	}

	// The results may be complex and are stored in an array
	// { x0, x0' * i, x1, x1' * i, ... }
	// First we check for complex solutions and throw in this case, then
	// we pick only the real solutions
	for (size_t ii = 0; ii < tmp.size(); ii += 2)
	{
		result.emplace_back(tmp[ii], tmp[ii+1]);
	}
	return result;
}

Polynomial::data_type
Polynomial::find_real_roots(float_type const val, bool in_domain) const
{
	data_type result;
	for (auto root : find_roots(val)) {
		if (root.imag() == 0.0) {
			if (!in_domain || Polynomial::in_domain(root.real(),getDomain())) {
				result.push_back(root.real());
			}
		}
	}
	return result;
}

size_t Polynomial::degree() const
{
	return mData.size() - 1;
}

Polynomial::data_type const&
Polynomial::getData() const
{
	return mData;
}

Polynomial::data_type&
Polynomial::getData()
{
	return mData;
}

bool
Polynomial::operator== (Transformation const& rhs) const
{
	Polynomial const* _rhs = dynamic_cast<Polynomial const*>(&rhs);
	if (!_rhs) {
		return false;
	}

	return (mData == _rhs->mData) && (mDomain == _rhs->mDomain);
}

std::ostream&
Polynomial::operator<< (std::ostream& os) const
{
	os << "Polynomial: ";
	for (int index = 0, size = mData.size(); index < size; ++index) {
		auto const& val = mData[index];
		if (index == 0)
		{
			os << val;
		}
		else
		{
			os << " + " << val << "*x^" << index;
		}
	}
	os << ", domain: " << mDomain;
	return os;
}

boost::shared_ptr<Polynomial>
Polynomial::create(data_type const& coeff,
		float_type const& min, float_type const& max)
{
	return boost::shared_ptr<Polynomial>(new Polynomial(coeff, min, max));
}

} // trafo
} // calibtic

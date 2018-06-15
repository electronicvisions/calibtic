#include "calibtic/trafo/Lookup.h"
#include "calibtic/trafo/Domain.h"
#include "calibtic/trafo/Polynomial.h"

#include <log4cxx/logger.h>
#include <algorithm>

static log4cxx::LoggerPtr _log = log4cxx::Logger::getLogger("Calibtic");

namespace calibtic {
namespace trafo {

Lookup::Lookup(data_type const& data, size_t offset) :
	mData(data),
	mOffset(offset),
	mSearchMode(determineSearchMode(data))
{
	if(mData.size()) {
		switch(mSearchMode) {
			case SEARCH_BINARY_RAISING:
				setDomain(mData.front(), mData.back());
				break;
			case SEARCH_BINARY_FALLING:
				setDomain(mData.back(), mData.front());
				break;
			default:
				throw std::runtime_error("calibtic::Lookup: Unreachable!");
		}
	}
}

Lookup::~Lookup() {}

float_type
Lookup::apply(float_type const& in, OutsideDomainBehavior outside_domain_behavior) const
{
	float_type val = respectDomain(in, outside_domain_behavior);

	data_type::const_iterator pos = mData.end();
	switch(mSearchMode) {
		case SEARCH_BINARY_RAISING:
			pos = std::lower_bound(mData.begin(), mData.end(), val);
			break;
		case SEARCH_BINARY_FALLING:
			pos = std::lower_bound(mData.begin(), mData.end(), val,
				                   std::greater_equal<data_type::value_type>());
			break;
		default:
			throw std::runtime_error("calibtic::Lookup: Unreachable!");
	}

	return std::distance(mData.begin(), pos) + mOffset - 1;
}

float_type
Lookup::reverseApply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior) const
{
	const int idx = respectReverseDomain(in, outside_domain_behavior) - mOffset;
	return mData.at(idx);
}

Lookup::data_type const&
Lookup::getData() const
{
	return mData;
}

bool
Lookup::operator== (Transformation const& rhs) const
{
	Lookup const* _rhs = dynamic_cast<Lookup const*>(&rhs);
	if (!_rhs) {
		return false;
	}

	return (mData == _rhs->mData) && (mOffset == _rhs->mOffset) &&
	       (mDomain == _rhs->mDomain);
}

std::ostream&
Lookup::operator<< (std::ostream& os) const
{
	os << "Lookup (offset: " << mOffset
	   << ", search mode: " << mSearchMode << "):\n";
	for (int index = 0, size = mData.size(); index < size; ++index) {
		os << "    " << index << ": " << mData[index] << "\n";
	}
	return os;
}

Lookup::search_mode Lookup::determineSearchMode(const data_type & data)
{
	if (std::is_sorted(data.begin(), data.end()))
		return SEARCH_BINARY_RAISING;
	if (std::is_sorted(data.begin(), data.end(),
                       std::greater_equal<data_type::value_type>()))
		return SEARCH_BINARY_FALLING;
	throw std::runtime_error("calibtic::Lookup: Provided data is not sorted!");
}

boost::shared_ptr<Lookup>
Lookup::create(data_type const& data, size_t offset)
{
	return boost::shared_ptr<Lookup>(new Lookup(data, offset));
}

} // trafo
} // calibtic

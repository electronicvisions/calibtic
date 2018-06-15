#include "calibtic/Collection.h"
#include <tuple>

//BOOST_CLASS_EXPORT_IMPLEMENT(calibtic::Collection)

namespace calibtic {

Collection::~Collection() {}

void Collection::insert(key_type const& key, value_type translation)
{
	auto ins = mBases.insert(std::make_pair(key, translation));

	if (!ins.second) {
		if (mBases.find(key) != mBases.end()) {
			throw std::runtime_error("key already exists");
		}

		throw std::runtime_error("unknown insertion error");
	}
}

void Collection::erase(key_type const& key)
{
	mBases.erase(key);
}

Collection::value_type Collection::at(key_type const& key)
{
	return mBases.at(key);
}

Collection::const_value_type Collection::at(key_type const& key) const
{
	return mBases.at(key);
}

Collection::size_type
Collection::size() const
{
	return mBases.size();
}


bool Collection::exists(key_type const& key) const
{
	return (mBases.find(key) != mBases.end());
}

bool Collection::operator== (Base const& rhs) const
{
	Collection const* _rhs = dynamic_cast<Collection const*>(&rhs);
	if (!_rhs) {
		return false;
	}
	return (*this == *_rhs);
}

bool Collection::operator== (Collection const& rhs) const
{
	bool equal = true;
	for (auto const& pair : mBases) {
		auto it = rhs.mBases.find(pair.first);
		if (!equal || it == rhs.mBases.end())
			return false;

		equal &= pair.second->operator==(*it->second);
	}
	return equal;
}

std::ostream& Collection::operator<< (std::ostream& os) const
{
	os << "Collection:" << std::endl;
	for (auto const& pair : mBases) {
		os << "\t" << *(pair.second) << std::endl;
	}
	return os;
}

boost::shared_ptr<Collection> Collection::create()
{
	return boost::shared_ptr<Collection>(new Collection());
}

void Collection::copy(Collection const& rhs)
{
	*this = rhs;
}

std::ostream& operator<< (std::ostream& os, Collection const& t)
{
	return t.operator<<(os);
}

} // calibtic

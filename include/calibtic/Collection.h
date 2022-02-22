#pragma once

#include <map>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>

// GCCXML has problems with atomics -> removed before shared_prt is included
#ifdef PYPLUSPLUS
#undef __ATOMIC_RELAXED
#undef __ATOMIC_ACQUIRE
#undef __ATOMIC_RELEASE
#endif // PYPLUSPLUS
#include <boost/serialization/shared_ptr.hpp>

#include "calibtic/Base.h"
#include "calibtic/Calibration.h"

namespace calibtic {

class Collection :
	public Base
{
public:
	typedef int key_type;
	typedef size_t size_type;

	typedef boost::shared_ptr<Base> value_type;
	typedef boost::shared_ptr<Base const> const_value_type;

private:
	typedef std::map<key_type, value_type> map_type;

public:
	virtual ~Collection();

	void insert(key_type const& key,
		   value_type translation);

	void erase(key_type const& key);

	value_type       at(key_type const& key);
#ifndef PYPLUSPLUS
	// 'at(key) const' needs to be removed for code generation, otherwise
	// #!&($-PY++ will emit code, which prefers const version over non const
	// version. However, PY++ is to stupid to register const shared_ptrs.
	const_value_type at(key_type const& key) const;
#endif // PYPLUSPLUS

	bool exists(key_type const& key) const;

	size_type size() const;

	virtual bool operator== (Base const& rhs) const;
	bool operator== (Collection const& rhs) const;

	virtual std::ostream& operator<< (std::ostream&) const;

	// factory function for Py++
	static
	boost::shared_ptr<Collection> create();

	virtual void copy(Collection const& rhs);

protected:
	map_type mBases;

private:
	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};


std::ostream& operator<< (std::ostream& os, Collection const& t);

} // calibtic



// implementations

namespace calibtic {

template<typename Archiver>
void Collection::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Base)
	   & make_nvp("translations", mBases);
}

} // calibtic

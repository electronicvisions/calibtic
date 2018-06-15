#pragma once

#include <ostream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>

namespace calibtic {

class Base
{
public:
	virtual ~Base();

#ifndef PYPLUSPLUS
	// operator== needs to be removed for code generation, otherwise
	// #!&($-PY++ will emit code, which tries to instantiate this
	// abstract class.
	virtual bool operator== (Base const&) const = 0;
#endif // PYPLUSPLUS
	virtual std::ostream& operator<< (std::ostream&) const = 0;

private:
	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver&, unsigned int const) {}
};

std::ostream& operator<< (std::ostream& os, Base const& t);

} // calibtic

BOOST_SERIALIZATION_ASSUME_ABSTRACT(calibtic::Base)

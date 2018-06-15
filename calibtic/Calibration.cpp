#include "calibtic/Calibration.h"
#include "calibtic/trafo/Transformation.h"

#include <sstream>
#include <iostream>

//BOOST_CLASS_EXPORT_IMPLEMENT(calibtic::Calibration)

namespace calibtic {

Calibration::Calibration(size_type const size, value_type const value) :
	mTrafo(size, value)
{
}

Calibration::~Calibration() {}

Calibration::const_value_type
Calibration::at(key_type const N) const
{
	if (!mTrafo.at(N)) {
		std::stringstream message;
		message << "No transformation available at " << N;
		throw std::runtime_error(message.str());
	}
	return mTrafo[N];
}

Calibration::value_type
Calibration::at(key_type const N)
{
	Calibration const& t = *this;
	return boost::const_pointer_cast<trafo_t>(t.at(N));
}

Calibration::size_type
Calibration::size() const
{
	return mTrafo.size();
}

void Calibration::reset(key_type const key, value_type trafo)
{
	mTrafo.at(key) = trafo;
}

void Calibration::reset(key_type const key, value_type& trafo)
{
	mTrafo.at(key) = trafo;
}

void Calibration::swap(key_type const key, value_type& trafo)
{
	mTrafo.at(key).swap(trafo);
}

bool Calibration::exists(key_type const& key) const
{
	return static_cast<bool>(mTrafo.at(key));
}

bool Calibration::operator== (Base const& rhs) const
{
	Calibration const* _rhs = dynamic_cast<Calibration const*>(&rhs);
	if (!_rhs) {
		return false;
	}
	return (*this == *_rhs);
}

bool Calibration::operator== (Calibration const& rhs) const
{
	size_type cnt = 0;
	for (auto const& val: mTrafo) {
		if (!val) {
			if (rhs.mTrafo.at(cnt)) {
				return false;
			}
		} else {
			if (!rhs.mTrafo.at(cnt) || !(*val == *(rhs.mTrafo.at(cnt)))) {
				return false;
			}
		}

		++cnt;
	}
	return true;
}

std::ostream& Calibration::operator<< (std::ostream& os) const
{
	os << "Calibration:" << std::endl;
	for (auto const& val : mTrafo) {
		if (val) {
			os << "\t" << *(val.get()) << std::endl;
		} else {
			os << "\tuninitialized" << std::endl;
		}
	}
	return os;
}

boost::shared_ptr<Calibration>
Calibration::create(size_type const size, value_type const value)
{
	return boost::shared_ptr<Calibration>(new Calibration(size, value));
}

void Calibration::copy(Calibration const& rhs)
{
	*this = rhs;
}

void Calibration::handleUninitialized(bool const init) const
{
	if (!init) {
		throw std::runtime_error("uninitialized data");
	}
}

std::ostream& operator<< (std::ostream& os, Calibration const& t)
{
	return t.operator<<(os);
}

} // calibtic

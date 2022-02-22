#pragma once

#include <memory>
#include <vector>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

// GCCXML has problems with atomics -> removed before boost serialization is included
#ifdef PYPLUSPLUS
#undef __ATOMIC_RELAXED
#undef __ATOMIC_ACQUIRE
#undef __ATOMIC_RELEASE
#endif // PYPLUSPLUS
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "calibtic/Base.h"
#include "calibtic/config.h"
#include "calibtic/trafo/Transformation.h"

namespace calibtic {

/// e.g. a Calibration corresponds to the Data of a single Neuron
class Calibration :
	public Base
{
public:
	typedef trafo::Transformation trafo_t;
	typedef boost::shared_ptr<trafo_t> value_type;
	typedef boost::shared_ptr<trafo_t const> const_value_type;
	typedef size_t key_type;
	typedef size_t size_type;

public:
	Calibration(size_type const size = 0, value_type const value = value_type());
	virtual ~Calibration();

	const_value_type at(key_type const N) const;
	value_type       at(key_type const N);

	size_type size() const;

	void reset(key_type const key, value_type trafo);
	void reset(key_type const key, value_type& trafo);
	void swap(key_type const key, value_type& trafo);

	bool exists(key_type const& key) const;

	virtual bool operator== (Base const& rhs) const;
	virtual bool operator== (Calibration const& rhs) const;
	virtual std::ostream& operator<< (std::ostream&) const;

	// factory function for Py++
	static
	boost::shared_ptr<Calibration>
	create(size_type const size = 0,
		   value_type const value = value_type());

	virtual void copy(Calibration const&);

protected:
	template<typename InputIt, typename OutputIt>
	void apply(
		InputIt first,
		InputIt const last,
		OutputIt dfirst) const;

	template <typename In, typename Out>
	void applyOne(In const& in, Out& out, key_type const offset = 0,
	              trafo_t::OutsideDomainBehavior outside_domain_behavior =
	                  trafo_t::CLIP) const;

	template <typename In, typename Out>
	void reverseApplyOne(In const& in, Out& out, key_type const offset = 0,
	                     trafo_t::OutsideDomainBehavior outside_domain_behavior =
	                         trafo_t::CLIP) const;

	std::vector<value_type> mTrafo;

private:
	void handleUninitialized(bool const init) const;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};


std::ostream& operator<< (std::ostream& os, Calibration const& t);

} // calibtic


// implementations
#ifndef PYPLUSPLUS

namespace calibtic {

template<typename InputIt, typename OutputIt>
void Calibration::apply(
	InputIt first,
	InputIt const last,
	OutputIt dfirst) const
{
	assert(int(mTrafo.size()) > (last-first-1));

	std::vector<value_type>::const_iterator it = mTrafo.begin();

	for (; first != last; ++first, ++it, ++dfirst)
	{
               handleUninitialized(static_cast<bool>(*it));

		*dfirst = (*it)->apply(*first);
	}
}

template <typename In, typename Out>
void Calibration::applyOne(
    In const& in, Out& out, key_type const offset,
    trafo_t::OutsideDomainBehavior outside_domain_behavior) const {
	assert(mTrafo.size() > offset);

	const_value_type val = mTrafo[offset];
	handleUninitialized(static_cast<bool>(val));

	out = val->apply(in, outside_domain_behavior);
}

template <typename In, typename Out>
void Calibration::reverseApplyOne(
    In const& in, Out& out, key_type const offset,
    trafo_t::OutsideDomainBehavior outside_domain_behavior) const {
	const_value_type val = mTrafo.at(offset);
	handleUninitialized(static_cast<bool>(val));

	out = val->reverseApply(in, outside_domain_behavior);
}

template<typename Archiver>
void Calibration::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & make_nvp("trafo", mTrafo);
}

} // calibtic
#endif // PYPLUSPLUS

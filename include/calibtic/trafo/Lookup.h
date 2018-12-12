#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>

#include "calibtic/trafo/Transformation.h"

namespace calibtic {
namespace trafo {

/// Does a binary search in monotonous data
/// Obeys Polynomial::outsideDomainBehavior
struct Lookup:
	public Transformation
{
	typedef std::vector<float_type> data_type;

	Lookup(data_type const& data = data_type(), size_t offset=0);
	virtual ~Lookup();

	virtual float_type
	apply(float_type const&, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::CLIP) const;

	virtual float_type
	reverseApply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::CLIP) const;

	virtual bool
	operator== (Transformation const& rhs) const;

	virtual std::ostream&
	operator<< (std::ostream& os) const;

	const data_type & getData() const;

	// factory function for Py++
	static
	boost::shared_ptr<Lookup>
	create(data_type const& data = data_type(), size_t offset=0);

private:
	enum search_mode
	{
		SEARCH_BINARY_FALLING,
		SEARCH_BINARY_RAISING,
	};

	data_type mData;
	size_t mOffset;
	search_mode mSearchMode;

	static search_mode determineSearchMode(const data_type & data);

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const)
	{
		using namespace boost::serialization;
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Transformation)
		   & make_nvp("data", mData)
		   & make_nvp("offset", mOffset)
		   & make_nvp("search_mode", mSearchMode);
	}
};

} // trafo
} // calibtic

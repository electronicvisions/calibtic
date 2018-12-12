#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

#include "calibtic/trafo/Transformation.h"

namespace calibtic {
namespace trafo {

struct Constant :
	public Transformation
{
	Constant(float_type const& val = 0.);
	virtual ~Constant();

	virtual float_type
	apply(float_type const&, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::IGNORE) const;

	virtual float_type
	reverseApply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::IGNORE) const;

	virtual bool
	operator== (Transformation const& rhs) const;

	virtual std::ostream&
	operator<< (std::ostream& os) const;

	float_type getData() const;

#ifndef PYPLUSPLUS
	float_type&       getData();
#endif // PYPLUSPLUS

	// factory function for Py++
	static
	boost::shared_ptr<Constant>
	create(float_type val = 0.);

private:
	float_type mData;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const)
	{
		using namespace boost::serialization;
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Transformation)
		   & make_nvp("data", mData);
	}
};

} // trafo
} // calibtic

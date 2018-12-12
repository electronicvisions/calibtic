#pragma once

#include <vector>
#include <ostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>

#include "calibtic/config.h"

#include "calibtic/trafo/Transformation.h"

namespace calibtic {
namespace trafo {

/// The sum of the result of other transformations.
class SumOfTrafos : public Transformation
{
public:

	typedef std::vector<boost::shared_ptr<Transformation> > trafo_list;

	SumOfTrafos(const trafo_list& trafos = trafo_list(),
				float_type const& min = CALIBTIC_DOMAIN_MIN,
				float_type const& max = CALIBTIC_DOMAIN_MAX);

	~SumOfTrafos() {};

	float_type apply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::CLIP) const;

	float_type reverseApply(float_type const& in, Transformation::OutsideDomainBehavior outside_domain_behavior=Transformation::CLIP) const;

	/// Also the order of the lhs's and rhs's transformations is compared.
	bool operator== (Transformation const& rhs) const;

	std::ostream& operator<< (std::ostream& os) const;

	// factory function for Py++
	static
	boost::shared_ptr<SumOfTrafos> create(trafo_list const& trafos);

private:

    trafo_list mTrafos;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const)
	{
		using namespace boost::serialization;
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Transformation)
			& make_nvp("trafos", mTrafos);
	}


};


} // trafo
} // calibtic


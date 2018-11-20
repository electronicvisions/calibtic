#pragma once

#include <iostream>
#include "calibtic/Collection.h"
#include "hal/Coordinate/HMFGeometry.h"

namespace HMF {

class SynapseSwitchCollection : public calibtic::Collection
{
public:
	SynapseSwitchCollection();
	virtual ~SynapseSwitchCollection();

	void setDefaults();

	size_t getMaxSwitches(HMF::Coordinate::VLineOnHICANN vline) const;
	void setMaxSwitches(HMF::Coordinate::VLineOnHICANN vline, size_t const s);

	// Py++ factory function
	static boost::shared_ptr<SynapseSwitchCollection> create();

	virtual void copy(Collection const& rhs);

private:
	friend class boost::serialization::access;
	template <typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};

} // HMF


// implementations

namespace HMF {

template <typename Archiver>
void SynapseSwitchCollection::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Collection);
}

} // HMF

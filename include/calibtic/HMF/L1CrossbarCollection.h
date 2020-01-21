#pragma once

#include <iostream>
#include "calibtic/Collection.h"
#include "halco/hicann/v2/l1.h"

namespace HMF {

class L1CrossbarCollection : public calibtic::Collection
{
public:
	L1CrossbarCollection();
	virtual ~L1CrossbarCollection();

	void setDefaults();

	template <typename T>
	size_t getMaxSwitchesPerRow(T line) const;

	template <typename T>
	void setMaxSwitchesPerRow(T line, size_t const s);

	template <typename T>
	size_t getMaxSwitchesPerColumn(T line) const;

	template <typename T>
	void setMaxSwitchesPerColumn(T line, size_t const s);

	// Py++ factory function
	static boost::shared_ptr<L1CrossbarCollection> create();

	virtual void copy(Collection const& rhs);

private:
	friend class boost::serialization::access;
	template <typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
	static const size_t VtoH_offset = halco::hicann::v2::VLineOnHICANN::max + 1; // cant have it const without being static. assuming we dont use heterogeneous hardware, static should not be a problem
};

} // HMF


// implementations

namespace HMF {

template <typename Archiver>
void L1CrossbarCollection::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Collection);
}

} // HMF

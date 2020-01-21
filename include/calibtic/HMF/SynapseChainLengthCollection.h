#pragma once

#include <iostream>
#include "calibtic/Collection.h"
#include "halco/hicann/v2/fwd.h"

namespace HMF {

class SynapseChainLengthCollection : public calibtic::Collection
{
public:
	SynapseChainLengthCollection();
	virtual ~SynapseChainLengthCollection();

	void setDefaults();

	size_t getMaxChainLength(halco::hicann::v2::VLineOnHICANN vline) const;
	void setMaxChainLength(halco::hicann::v2::VLineOnHICANN vline, size_t const s);

	// Py++ factory function
	static boost::shared_ptr<SynapseChainLengthCollection> create();

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
void SynapseChainLengthCollection::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Collection);
}

} // HMF

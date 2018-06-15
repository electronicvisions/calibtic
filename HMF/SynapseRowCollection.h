#pragma once

#include "calibtic/Collection.h"

namespace HMF {

class SynapseRowCollection :
	public calibtic::Collection
{
public:
	SynapseRowCollection();
	virtual ~SynapseRowCollection();

	void setDefaults();
	/// sets 4 different synapse calibrations for 4 gmax_config settings,
	/// to allow a wider weight range in ESS simulations.
	/// cf. SynapseRowCalibration.setEssDefaults()
	void setEssDefaults();

	virtual std::ostream& operator<< (std::ostream&) const;

	// Py++ factory function
	static
	boost::shared_ptr<SynapseRowCollection> create();

	virtual void copy(calibtic::Collection const&);

private:
	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);

};

} // HMF

// implementations

namespace HMF {

template<typename Archiver>
void SynapseRowCollection::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Collection);
}

} // HMF

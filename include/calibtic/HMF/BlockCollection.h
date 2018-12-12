#pragma once

#include "calibtic/Collection.h"
#include "calibtic/HMF/SharedCalibration.h"
#include "calibtic/HMF/HWNeuronParameter.h"

namespace HMF {

class BlockCollection :
	public calibtic::Collection
{
public:
	BlockCollection();
	virtual ~BlockCollection();

	void setDefaults();

	virtual std::ostream& operator<< (std::ostream&) const;

	// Py++ factory function
	static
	boost::shared_ptr<BlockCollection> create();

	virtual void copy(calibtic::Collection const&);

	HWSharedParameter applySharedCalibration(double v_reset, size_t hw_shared_id) const;

private:
	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);

};

} // HMF

// implementations

namespace HMF {

template<typename Archiver>
void BlockCollection::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Collection);
}

} // HMF

#pragma once

#include <iostream>
#include "calibtic/Calibration.h"

namespace HMF {

class SynapseChainLengthCalibration : public calibtic::Calibration
{
public:
	SynapseChainLengthCalibration();
	virtual ~SynapseChainLengthCalibration();

	void setDefaults();

	size_t getMaxChainLength() const;
	void setMaxChainLength(size_t const s);

	virtual std::ostream& operator<<(std::ostream&) const;

	// Py++ factory function
	static boost::shared_ptr<SynapseChainLengthCalibration> create();

	virtual void copy(Calibration const& rhs);

private:
	friend class boost::serialization::access;
	template <typename Archiver>
	void serialize(Archiver& ar, unsigned int const);

	size_t mMaxChainLength;
};

} // HMF


// implementations

namespace HMF {

template <typename Archiver>
void SynapseChainLengthCalibration::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(Calibration) &
	    make_nvp("MaxChainLength", mMaxChainLength);
}

} // HMF

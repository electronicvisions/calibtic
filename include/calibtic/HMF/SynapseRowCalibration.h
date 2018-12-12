#pragma once
#include <stdexcept>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <iostream>

#include "calibtic/Base.h"
#include "hal/HICANNContainer.h"
#include "calibtic/HMF/SynapseCalibration.h"
#include "calibtic/HMF/GmaxConfig.h"

namespace HMF {

// holds different synapse calibrations for one synapse row,
// for different settings of sel_Vgmax and gmax_divisor.
// In order to have only one synapse transformation per setting,
// the configuration of sel_Vgmax and gmax_divisor are stored as the keys for the SynapseTransformations.
// TODO:
//  * currently, the insert/erase/at/at design was adopted from calibtic::Collection. We might re-think the design.
class SynapseRowCalibration :
	public calibtic::Base
{
public:
	typedef HMF::SynapseCalibration trafo_t;
	typedef boost::shared_ptr<trafo_t> value_type;
	typedef boost::shared_ptr<trafo_t const> const_value_type;


	typedef GmaxConfig key_type;
	typedef size_t size_type;

public:
	virtual ~SynapseRowCalibration();

	void setDefaults();

	/// sets 4 different synapse calibrations for 4 gmax_config settings, to allow a wider weight range in ESS simulations
	void setEssDefaults();

	// get synapse calibration.
	// raises exception if there is no calibration for this gmax config
	const_value_type at(key_type const key) const;
	value_type       at(key_type const key);

	// gets number of existing calibrations
	size_type size() const;

	// insert new calibration for a new gmax config (key).
	// raises exception if there already exists a calibration for the given key.
	void insert(key_type const& key,
		   value_type val);

	// erase synapse calibration for given key. returns number of calibrations removed (0 or 1).
	size_type erase(key_type const& key);

	// clear all synapse calibrations
	void clear();

	// checks whether a calibration for a given gmax config (key) exists.
	bool exists(key_type const& key) const;

	/// returns best GmaxConfig.
	/// This refers to the config, for which maximum required weight is smaller than the maximum analog weight,
	/// but as close to the maximum as possible.
	/// If required weight is higher than the maximum analog weight, the Configuration with the highest maximum
	/// weight is returned.
	/// If no synapse calibration is available, the default setting is returned.
	GmaxConfig findBestGmaxConfig(double max_required_weight);

	virtual bool operator== (Base const& rhs) const;
	virtual bool operator== (SynapseRowCalibration const& rhs) const;
	virtual std::ostream& operator<< (std::ostream&) const;

	// factory function for Py++
	static
	boost::shared_ptr<SynapseRowCalibration> create();

	virtual void copy(SynapseRowCalibration const&);

protected:
	std::map<key_type, value_type> mTrafo;

private:
	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};


std::ostream& operator<< (std::ostream& os, SynapseRowCalibration const& t);


// implementations
#ifndef PYPLUSPLUS

template<typename Archiver>
void SynapseRowCalibration::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & make_nvp("trafo", mTrafo);
}
#endif // PYPLUSPLUS

} // HMF

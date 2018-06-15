#include "HMF/SynapseRowCalibration.h"
#include "calibtic/trafo/Transformation.h"
#include "calibtic/trafo/Polynomial.h"

#include <iostream>

#include <log4cxx/logger.h>

static log4cxx::LoggerPtr _log = log4cxx::Logger::getLogger("Calibtic");

namespace HMF {

SynapseRowCalibration::~SynapseRowCalibration() {}

void SynapseRowCalibration::setDefaults()
{
	boost::shared_ptr<SynapseCalibration> sc(new SynapseCalibration);
	sc->setDefaults();
	mTrafo[GmaxConfig::Default()] = sc;
}

void SynapseRowCalibration::setEssDefaults()
{
	clear();
	setDefaults();
	boost::shared_ptr<SynapseCalibration> sc = mTrafo[GmaxConfig::Default()];
	boost::shared_ptr< calibtic::trafo::Polynomial > default_trafo =  boost::static_pointer_cast<calibtic::trafo::Polynomial >(sc->at(0));
	auto default_coeffs = default_trafo->getData();

	// The default trafo has used gmax_divisor=11 and V_gmax=500 nA.
	double vgmax_default = 500.;
	uint8_t gmax_div_default = 11;

	// We now generate 3 other calibrations with other settings for gmax_divisor and V_gmax.
	//
	// These are chosen to cover the weight range promised to the BSS Modelers, see:
	// https://brainscales-r.kip.uni-heidelberg.de/projects/bss-neuron-parameters/repository/revisions/master/entry/cells.py
	// max_weight in bio domain for cm=0.2 nS: 300 nS
	// UPDATE by BV: chosen such that max weight is 100 nS, as 300 nS seem unrealistically high.
	//
	// To generate some meaningfull data, we assume that the analog weight is proportional to V_gmax and to 1/gmax_divisor,
	// and choose artificial settings from V_gmax and gmax_div
	// The dependency on the digital weights is copied from the existing default trafo.
	std::vector< std::pair<double,uint8_t> > gmax_settings = {{550.,1}, {650.,4}, {200.,15}}; // first: V_gmax in nA, second: gmax_divisor 

	uint8_t sel_gmax = 1;
	for (auto vgmax_and_div : gmax_settings ) {
		double vgmax_new = vgmax_and_div.first;
		uint8_t gmax_div_new = vgmax_and_div.second;
		double scale_factor = (vgmax_new/vgmax_default)*(gmax_div_default*1./gmax_div_new);
		// polynomial coefficient are scaled by that factor
		std::vector<double> new_coeffs;
		for (auto coeff:default_coeffs)
			new_coeffs.push_back(coeff*scale_factor);
		// add entries
		GmaxConfig gc(sel_gmax,gmax_div_new);
		boost::shared_ptr<SynapseCalibration> sc(new SynapseCalibration);
		sc->reset(0, calibtic::trafo::Polynomial::create(new_coeffs, HICANN::SynapseWeight::min, HICANN::SynapseWeight::max));
		mTrafo[ gc ] = sc;
		sel_gmax++;
	}
}


SynapseRowCalibration::const_value_type
SynapseRowCalibration::at(SynapseRowCalibration::key_type const key) const
{
	if ( !exists(key) ) {
		std::stringstream ss; ss << "SynapseRowCalibration: no SynapseCalibration available for key = " << key ;
		throw std::runtime_error(ss.str());
	}
	return mTrafo.at(key);
}

SynapseRowCalibration::value_type
SynapseRowCalibration::at(SynapseRowCalibration::key_type const key)
{
	SynapseRowCalibration const& t = *this;
	return boost::const_pointer_cast<trafo_t>(t.at(key));
}

SynapseRowCalibration::size_type
SynapseRowCalibration::size() const
{
	return mTrafo.size();
}

void SynapseRowCalibration::insert(key_type const& key, value_type value)
{
	auto ins = mTrafo.insert(std::make_pair(key, value));

	if (!ins.second) {
		throw std::runtime_error("SynapseRowCalibration::insert(..): key already exists");
	}
}

SynapseRowCalibration::size_type
SynapseRowCalibration::erase(key_type const& key)
{
	return mTrafo.erase(key);
}

void SynapseRowCalibration::clear()
{
	mTrafo.clear();
}

bool SynapseRowCalibration::exists(key_type const& key) const
{
	return mTrafo.count(key);
}

bool SynapseRowCalibration::operator== (Base const& rhs) const
{
	SynapseRowCalibration const* _rhs = dynamic_cast<SynapseRowCalibration const*>(&rhs);
	if (!_rhs) {
		return false;
	}
	return (*this == *_rhs);
}

bool SynapseRowCalibration::operator== (SynapseRowCalibration const& rhs) const
{
	return size() == rhs.size()
		&&  std::equal(mTrafo.begin(),mTrafo.end(), rhs.mTrafo.begin()) ;
}

std::ostream& SynapseRowCalibration::operator<< (std::ostream& os) const
{
	os << "SynapseRowCalibration:" << std::endl;
	for (auto const& val : mTrafo) {
		os << val.first << "\t" << val.second << std::endl;
	}
	return os;
}

boost::shared_ptr<SynapseRowCalibration>
SynapseRowCalibration::create()
{
	return boost::shared_ptr<SynapseRowCalibration>(new SynapseRowCalibration());
}

void SynapseRowCalibration::copy(SynapseRowCalibration const& rhs)
{
	*this = rhs;
}

std::ostream& operator<< (std::ostream& os, SynapseRowCalibration const& t)
{
	return t.operator<<(os);
}


GmaxConfig
SynapseRowCalibration::findBestGmaxConfig(double max_required_weight)
{
	if (mTrafo.size() == 0) {
		LOG4CXX_WARN(_log, "SynapseRowCalibration::findBestGmaxConfig(): no synapse calibration available, restoring default calibration");
		setDefaults();
	}
	GmaxConfig rv(0,0);
	if (mTrafo.size() == 1) {
		rv = mTrafo.begin()->first;
	}
	else {
		// get configuration so that
		// 1) max_required_weight <= max_analog_weight
		// 2) absolute difference (max_analog_weight - max_required_weight) is minimal
		// if 1) cannot be fulfilled, only 2) applies.

		// Searching for 1) first.
		std::map<key_type, value_type> candidates_fullfilling_1;
		for (auto const & trafo : mTrafo ) {
			if (max_required_weight <= trafo.second->getMaxAnalogWeight())
				candidates_fullfilling_1.insert(trafo);
		}

		// If no candidate fulfills 1), use all for 2)
		std::map<key_type, value_type>* canditates_for_2;
		if (candidates_fullfilling_1.size() > 0)
			canditates_for_2 = & candidates_fullfilling_1;
		else
			canditates_for_2  = & mTrafo;

		// Search for 2)
		double lowest_abs_diff = std::numeric_limits<double>::infinity();
		for (auto const & trafo : *canditates_for_2 ) {
			double abs_diff = std::abs(max_required_weight - trafo.second->getMaxAnalogWeight());
			if ( abs_diff < lowest_abs_diff ){
				lowest_abs_diff  = abs_diff;
				rv = trafo.first;
			}
		}

	}
	LOG4CXX_DEBUG(_log, "SynapseRowCalibration::findBestGmaxConfig(max_required_weight=" << max_required_weight << "): found GmaxConfig " << rv << "with min and max weight " << mTrafo[rv]->getMinAnalogWeight() << " and " << mTrafo[rv]->getMaxAnalogWeight() );
	return rv;
}

} // HMF

#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/export.hpp>
#include <iostream>

//#include "rant/int.h"
#include "pywrap/compat/rant.hpp"

namespace HMF {


/// class holds the gmax configuration for one synapse row
/// i.e. different settings of sel_Vgmax and gmax_divisor.
struct GmaxConfig {
private:
	typedef rant::integral_range<uint8_t, 3> sel_Vgmax_t;
	typedef rant::integral_range<uint8_t, 15> gmax_div_t;

public:
	GmaxConfig(uint8_t sel, uint8_t div)
		:sel_Vgmax(sel), gmax_div(div)
	{}
	GmaxConfig() :sel_Vgmax(0), gmax_div(1)
	{}
	virtual ~GmaxConfig(){}

	uint8_t get_sel_Vgmax() const;
	void set_sel_Vgmax(uint8_t v);

	uint8_t get_gmax_div() const;
	void set_gmax_div(uint8_t v);

	bool operator <(GmaxConfig const rhs) const {
		return sel_Vgmax == rhs.sel_Vgmax ?  gmax_div < rhs.gmax_div : sel_Vgmax < rhs.sel_Vgmax ;
	}
	bool operator ==(GmaxConfig const rhs) const {
		return (sel_Vgmax == rhs.sel_Vgmax && gmax_div == rhs.gmax_div);
	}
	std::ostream& operator<< (std::ostream& os) const {
		os << "GmaxConfig(sel_Vgmax=" <<  (unsigned int) get_sel_Vgmax() << ", gmax_div=" <<  (unsigned int) get_gmax_div() << ")";
		return os;
	}
	static GmaxConfig Default() {
		return GmaxConfig(0,11);
	}

private:
	// members
	sel_Vgmax_t sel_Vgmax;
	gmax_div_t gmax_div;

	friend class boost::serialization::access;
	template<typename Archiver>
	void serialize(Archiver& ar, unsigned int const);
};

std::ostream& operator<< (std::ostream& os, GmaxConfig const& t);

// implementations
#ifndef PYPLUSPLUS

template<typename Archiver>
void GmaxConfig::serialize(Archiver& ar, unsigned int const)
{
	using namespace boost::serialization;
	ar & make_nvp("sel_Vgmax", sel_Vgmax)
	   & make_nvp("gmax_div", gmax_div);
}
#endif // PYPLUSPLUS


} // HMF

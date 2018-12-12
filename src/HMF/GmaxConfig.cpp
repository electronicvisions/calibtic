#include "calibtic/HMF/GmaxConfig.h"

namespace HMF {

uint8_t GmaxConfig::get_sel_Vgmax() const
{
	return sel_Vgmax;
}

void GmaxConfig::set_sel_Vgmax(uint8_t v)
{
	sel_Vgmax = v;
}

uint8_t GmaxConfig::get_gmax_div() const
{
	return gmax_div;
}

void GmaxConfig::set_gmax_div(uint8_t v)
{
	gmax_div = v;
}

std::ostream& operator<< (std::ostream& os, GmaxConfig const& t)
{
	return t.operator<<(os);
}

} // HMF

#include "calibtic/HMF/HWSharedParameter.h"

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.h>
#include <boost/preprocessor/cat.hpp>

// halbe api
#include "hal/HICANN/FGControl.h"
#include "logger.h"

#define HICANN_PARAM_SEQ BOOST_PP_VARIADIC_TO_SEQ(HICANN_PARAM_VARIADIC)

static log4cxx::LoggerPtr _log = log4cxx::Logger::getLogger("Calibtic");

namespace HMF {

HWSharedParameter::HWSharedParameter() :
	mParam(size())
{}

HWSharedParameter::HWSharedParameter(std::vector<value_type> const& param) :
	mParam(param)
{
	if (size() != param.size()) {
		throw std::runtime_error("wrong number of hw parameters");
	}
}

HWSharedParameter::value_type
HWSharedParameter::getParam(int const ii) const
{
	return mParam.at(ii);
}

void HWSharedParameter::setParam(int const ii, value_type const val)
{
	mParam.at(ii) = val;
}

std::vector<HWSharedParameter::value_type> const&
HWSharedParameter::parameters() const
{
	return mParam;
}

std::vector<HWSharedParameter::value_type>&
HWSharedParameter::parameters()
{
	return mParam;
}

size_t HWSharedParameter::size()
{
	return HICANN::shared_parameter::__last_shared;
}

void HWSharedParameter::toHW(HMF::Coordinate::FGBlockOnHICANN const& fgb, HICANN::FGControl& fg) const
{
	auto& log = Logger::instance();

	// fill floating gates from internal parameter vector
	using HMF::HICANN::shared_parameter;
	for (int pp=0; pp<shared_parameter::__last_shared; ++pp)
	{

		// some parameters are local to the top/bottom FG blocks

		if(fgb.toSideHorizontal() == HMF::Coordinate::top &&
		   (pp == shared_parameter::V_bexp || pp == shared_parameter::V_clrc)) {
		    continue;
		}

		if(fgb.toSideHorizontal() == HMF::Coordinate::bottom &&
		   (pp == shared_parameter::V_bout || pp == shared_parameter::V_clra)) {
		    continue;
		}

		value_type val = mParam.at(pp);
		if (val > 1023) {
			log(Logger::WARNING) << "clipping parameter " << pp << " from " << val << " to 1023";
			val = 1023;
		} else if (val < 0) {
			log(Logger::WARNING) << "clipping parameter " << pp << " from " << val << " to 0";
			val = 0;
		}
		try {
			fg.setShared(fgb, shared_parameter(pp), val);
		} catch(const std::exception& e) {
		  	LOG4CXX_WARN(_log, "trouble setting shared parameter number " << pp << " on: " << fgb << " because: " << e.what());
		}

	}
}

void HWSharedParameter::fromHW(HMF::Coordinate::FGBlockOnHICANN const& fgb, HICANN::FGControl const& fg)
{
	using HMF::HICANN::shared_parameter;
	for (int pp=0; pp<shared_parameter::__last_shared; ++pp)
		try {
			mParam.at(pp) = fg.getShared(fgb, shared_parameter(pp));
		} catch(const std::exception& e) {
			LOG4CXX_WARN(_log, "trouble getting shared parameter number " << pp << " because: " << e.what());
		}
}

} // HMF

namespace HMF {
std::ostream& operator<<(std::ostream& os, HMF::HWSharedParameter const& p)
{
	static std::array<std::string, 23> const names {{
	"V_reset",
	"int_op_bias",
	"V_dllres",
	"V_bout",
	"V_bexp",
	"V_fac",
	"I_breset",
	"V_dep",
	"I_bstim",
	"V_thigh",
	"V_gmax3",
	"V_tlow",
	"V_gmax0",
	"V_clra",
	"V_clrc",
	"V_gmax1",
	"V_stdf",
	"V_gmax2",
	"V_m",
	"V_bstdf",
	"V_dtc",
	"V_br",
	"V_ccas"
	}};

	auto const& v = p.parameters();
	for (size_t ii=0; ii<v.size(); ++ii)
	{
		os << names.at(ii) << "\t" << v[ii] << '\n';
	}
	return os;
}
}

#undef HICANN_PARAM_SEQ

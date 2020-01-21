#pragma once

#include <cstdlib>
#include <vector>
#include "halco/hicann/v2/fwd.h"
#include "hal/HICANN/FGBlock.h" // for parameter enums
#include "hal/HICANN/FGControl.h"

namespace HMF {

namespace HICANN {
struct FGControl; // fwd
enum shared_parameter; // fwd
} // HICANN

// TODO: add common base class for HWShared/NeuronParameter?
class HWSharedParameter
{
public:
	typedef int value_type;

	HWSharedParameter();
	HWSharedParameter(std::vector<value_type> const& param);

	static size_t size();

	value_type getParam(int const ii) const;
	void setParam(int const ii, value_type const val);

#ifndef PYPLUSPLUS
	std::vector<value_type> const& parameters() const;
	std::vector<value_type>&       parameters();
#endif

	void toHW(halco::hicann::v2::FGBlockOnHICANN const& fgb, HICANN::FGControl& fg) const;
	void fromHW(halco::hicann::v2::FGBlockOnHICANN const& fgb, HICANN::FGControl const& fg);

private:
	std::vector<value_type> mParam;
};

} // HMF

#ifndef PYPLUSPLUS
namespace HMF {
std::ostream& operator<<(std::ostream& os, HMF::HWSharedParameter const& p);
}
#endif

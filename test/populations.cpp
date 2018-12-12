#include "test.h"
#include <stdexcept>
#include <array>

#include "euter/objectstore.h"
#include "euter/population_view.h"
#include "euter/projection_view.h"
#include "euter/fixedprobabilityconnector.h"
#include "euter/nativerandomgenerator.h"
#include <boost/make_shared.hpp>

#include "calibtic/HMF/NeuronCalibration.h"

using namespace HMF;

std::array<std::string, 21> const names {{
	"E_l", "E_syni", "E_synx", "I_bexp", "I_convi",
	"I_convx", "I_fire", "I_gl", "I_gladapt", "I_intbbi",
	"I_intbbx", "I_pl", "I_radapt", "I_rexp", "I_spikeamp",
	"V_exp", "V_syni", "V_syntci", "V_syntcx", "V_synx", "V_t"
}};

struct NeuronTestVisitor
{
    typedef void return_type;
	typedef NeuronCalibration calib_t;

    template <CellType N>
        using cell_t = TypedCellParameterVector<N>;

    template <CellType N>
    return_type operator() (
        cell_t<N> const&,
        calib_t const&) const
    {
		throw std::runtime_error("unsupported neuron type");
    }

    // AdEx Parameter Transformation
    return_type operator() (
        cell_t<CellType::EIF_cond_exp_isfa_ista> const& v,
        calib_t const& calib) const
	{
		apply(v, calib);
	}

    // LIF Parameter Transformation
    return_type operator() (
        cell_t<CellType::IF_cond_exp> const& v,
        calib_t const& calib) const
	{
		apply(v, calib);
	}

private:
	template<typename T>
	void apply(T const& v, calib_t const& calib) const
	{
		auto const& params = v.parameters();
		for (size_t ii=0; ii<params.size(); ++ii)
		{
			auto hw = calib.applyNeuronCalibration(params[ii], 10000);
			for (size_t pp=0; pp<hw.parameters().size(); ++pp)
			{
				auto const val = hw.parameters()[pp];
				EXPECT_LE(0., val) << names.at(pp) << ": " << val;
			}
		}
	}
};

class PopulationWithCellType : public ::testing::TestWithParam<CellType>
{
public:
	ObjectStore os;
	NeuronCalibration nc;
	PopulationPtr pop;

	void SetUp()
	{
		nc.setDefaults();

		pop = Population::create(os, 1+rand()%10, GetParam());
	}
};

TEST_P(PopulationWithCellType, HasSensibleDefaultCalibration)
{
	NeuronTestVisitor visitor;
    visitCellParameterVector(pop->parameters(), visitor, nc);
}

INSTANTIATE_TEST_CASE_P(Common,
                        PopulationWithCellType,
                        ::testing::Values(
	                        CellType::IF_cond_exp,
	                        CellType::EIF_cond_exp_isfa_ista));

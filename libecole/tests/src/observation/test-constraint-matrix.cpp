#include <catch2/catch.hpp>
#include <xtensor/xview.hpp>

#include "ecole/observation/constraint-matrix.hpp"

#include "conftest.hpp"
#include "observation/unit-tests.hpp"

using namespace ecole;

TEST_CASE("ConstraintMatrix unit tests", "[unit][obs]") {
	observation::unit_tests(observation::ConstraintMatrix{});
}

TEST_CASE("ConstraintMatrix return correct observation", "[obs]") {
	auto obs_func = observation::ConstraintMatrix{};
	auto model = get_solving_model();
	obs_func.before_reset(model);
	auto const obs = obs_func.extract(model, false);

	SECTION("Observation shape match model") {
		REQUIRE(obs.shape[0] >= model.lp_rows().size());  // Row may be doubled if they have two bounds
		REQUIRE(obs.shape[1] == model.lp_columns().size());
	}

	SECTION("Constraint sparse indices are within range") {
		REQUIRE(xt::all(xt::row(obs.indices, 0) < obs.shape[0]));
		REQUIRE(xt::all(xt::row(obs.indices, 1) < obs.shape[1]));
	}

	SECTION("Observations do not change") {
		model.solve_iter_branch(model.lp_branch_cands()[0]);
		auto const next_obs = obs_func.extract(model, model.solve_iter_is_done());
		REQUIRE(obs.shape == next_obs.shape);
		REQUIRE(obs.nnz() == next_obs.nnz());
		REQUIRE(obs.indices == next_obs.indices);
		REQUIRE(obs.values == next_obs.values);
	}
}

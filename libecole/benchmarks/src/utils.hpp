#pragma once

#include <benchmark/benchmark.h>

#include "ecole/scip/model.hpp"

#ifndef TEST_DATA_DIR
#error "Need to define TEST_DATA_DIR."
#endif
constexpr auto problem_file = (TEST_DATA_DIR "/bppc8-02.mps");

namespace ecole {

inline auto get_model() {
	auto model = scip::Model::from_file(problem_file);
	model.disable_cuts();
	model.disable_presolve();
	constexpr auto seed = 784;
	model.set_param("randomization/permuteconss", true);
	model.set_param("randomization/permutevars", true);
	model.set_param("randomization/permutationseed", seed);
	model.set_param("randomization/randomseedshift", seed);
	model.set_param("randomization/lpseed", seed);
	return model;
}

}  // namespace ecole

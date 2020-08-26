#include <benchmark/benchmark.h>
#include <scip/scip.h>

#include "index-branchrule.hpp"
#include "utils.hpp"

using namespace ecole;

template <typename Func> auto benchmark_solve(benchmark::State& state, Func&& func_to_bench) {
	auto n_nodes = ecole::scip::long_int{0};
	auto n_lp_iterations = ecole::scip::long_int{0};

	for (auto _ : state) {
		state.PauseTiming();
		auto model = get_model();

		state.ResumeTiming();
		func_to_bench(model);
		state.PauseTiming();

		n_nodes += SCIPgetNTotalNodes(model.get_scip_ptr());
		n_lp_iterations += SCIPgetNLPIterations(model.get_scip_ptr());
	}

	using benchmark::Counter;
	state.counters["Nodes"] = Counter{static_cast<double>(n_nodes), Counter::kAvgIterations};
	state.counters["LP Iterations"] = Counter{static_cast<double>(n_lp_iterations), Counter::kAvgIterations};
}

auto presolve(benchmark::State& state) {
	benchmark_solve(state, [](auto& model) { model.presolve(); });
}
BENCHMARK(presolve)  // NOLINT(cert-err58-cpp)
	->MeasureProcessCPUTime()
	->UseRealTime()
	->Unit(benchmark::kMillisecond);

auto ecole_reverse_control(benchmark::State& state) {
	benchmark_solve(state, [](auto& model) {
		model.solve_iter();
		while (!model.solve_iter_is_done()) {
			model.solve_iter_branch(model.lp_branch_cands()[0]);
		}
	});
}
BENCHMARK(ecole_reverse_control)  // NOLINT(cert-err58-cpp)
	->MeasureProcessCPUTime()
	->UseRealTime()
	->Unit(benchmark::kMillisecond);

auto scip_branch_rule(benchmark::State& state) {
	benchmark_solve(state, [](auto& model) {
		auto branch_rule = std::make_unique<ecole::scip::IndexBranchrule>(model.get_scip_ptr(), "FirstVarBranching", 0UL);
		SCIPincludeObjBranchrule(model.get_scip_ptr(), branch_rule.release(), true);
		model.solve();
	});
}
BENCHMARK(scip_branch_rule)  // NOLINT(cert-err58-cpp)
	->MeasureProcessCPUTime()
	->UseRealTime()
	->Unit(benchmark::kMillisecond);

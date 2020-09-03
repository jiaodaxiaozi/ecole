#include <benchmark/benchmark.h>
#include <scip/scip.h>

#include "ecole/observation/nodebipartite.hpp"

#include "utils.hpp"

using namespace ecole;

template <typename ObsFunc> auto benchmark_observation(benchmark::State& state, ObsFunc&& func_to_bench) {
	auto n_nodes = ecole::scip::long_int{0};

	for (auto _ : state) {
		state.PauseTiming();
		auto model = get_model();

		model.solve_iter();

		state.ResumeTiming();
		func_to_bench.reset(model);
		state.PauseTiming();

		while (!model.solve_iter_is_done()) {
			model.solve_iter_branch(model.lp_branch_cands()[0]);

			state.ResumeTiming();
			benchmark::DoNotOptimize(func_to_bench.obtain_observation(model));
			state.PauseTiming();
		}

		state.PauseTiming();
		n_nodes += SCIPgetNTotalNodes(model.get_scip_ptr());
	}

	using benchmark::Counter;
	state.counters["Nodes"] = Counter{static_cast<double>(n_nodes), Counter::kAvgIterations};
}

auto node_bipartite_episode(benchmark::State& state) {
	benchmark_observation(state, observation::NodeBipartite{});
}
BENCHMARK(node_bipartite_episode)  // NOLINT(cert-err58-cpp)
	->MeasureProcessCPUTime()
	->UseRealTime()
	->Unit(benchmark::kMillisecond);

import pathlib

import google_benchmark as benchmark
import pyscipopt

import ecole

TEST_SOURCE_DIR = pathlib.Path(__file__).parent.resolve()
DATA_DIR = TEST_SOURCE_DIR / "../../libecole/tests/data"


def get_model():
    """Return a Model object with a valid problem."""
    model = ecole.scip.Model.from_file(str(DATA_DIR / "bppc8-02.mps"))
    model.disable_cuts()
    model.disable_presolve()
    model.set_param("randomization/permuteconss", True)
    model.set_param("randomization/permutevars", True)
    model.set_param("randomization/permutationseed", 784)
    model.set_param("randomization/randomseedshift", 784)
    model.set_param("randomization/lpseed", 784)
    return model


def benchmark_model_solving(func_to_bench):
    @benchmark.register(name=func_to_bench.__name__)
    @benchmark.option.measure_process_cpu_time()
    @benchmark.option.use_real_time()
    @benchmark.option.unit(benchmark.kMillisecond)
    def _benchmark(state):
        n_nodes = 0
        n_lp_iterations = 0
        while state:
            state.pause_timing()
            model = get_model()

            state.resume_timing()
            func_to_bench(model)
            state.pause_timing()

            n_nodes += model.as_pyscipopt().getNNodes()  # FIXME different from C++
            n_lp_iterations += model.as_pyscipopt().getNLPIterations()  # FIXME Not all nodes
            state.resume_timing()

        state.counters["Nodes"] = benchmark.Counter(n_nodes, benchmark.Counter.kAvgIterations)
        state.counters["LP Iterations"] = benchmark.Counter(
            n_lp_iterations, benchmark.Counter.kAvgIterations
        )

    return _benchmark


@benchmark_model_solving
def presolve(model):
    model.presolve()


@benchmark_model_solving
def ecole_reverse_control(model):
    model.solve_iter()
    while not model.solve_iter_is_done():
        model.solve_iter_branch(model.lp_branch_cands[0])


if __name__ == "__main__":
    benchmark.main()

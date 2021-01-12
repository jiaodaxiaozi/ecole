import google_benchmark as benchmark
import pyscipopt

from index_branchrule import IndexBranchrule
from utils import get_model


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


@benchmark_model_solving
def pyscipopt_branchrule(model):
    pyscipopt_model = model.as_pyscipopt()
    pyscipopt_model.includeBranchrule(
        IndexBranchrule(pyscipopt_model, 0),
        "IndexBranchrule",
        "Branch on first LP branch cand",
        priority=536870911,
        maxdepth=-1,
        maxbounddist=1.0,
    )
    # Call `SCIPsolve` from PySCIPOpt because Ecole releases the GIL which is invalid if the Branchrule is written in
    # Python.
    pyscipopt_model.optimize()


if __name__ == "__main__":
    benchmark.main()

import pyscipopt


class IndexBranchrule(pyscipopt.Branchrule):
    def __init__(self, model: pyscipopt.Model, index: int) -> None:
        self.model = model
        self.index = index

    def branchexeclp(self, allowaddcons: bool):
        var = self.model.getLPBranchCands()[0][self.index]
        self.model.branchVar(var)
        return {"result": pyscipopt.SCIP_RESULT.BRANCHED}

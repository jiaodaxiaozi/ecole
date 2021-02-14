#include "ecole/reward/dualbound.hpp"
#include "ecole/scip/model.hpp"

namespace ecole::reward {

static auto dual_bound(scip::Model const& model) {
	switch (model.get_stage()) {
	// Only stages when the following call is authorized
	case SCIP_STAGE_TRANSFORMED:
  case SCIP_STAGE_INITPRESOLVE:
  case SCIP_STAGE_PRESOLVING:
  case SCIP_STAGE_EXITPRESOLVE:
  case SCIP_STAGE_PRESOLVED:
  case SCIP_STAGE_INITSOLVE:
  case SCIP_STAGE_SOLVING:
  case SCIP_STAGE_SOLVED:
		return SCIPgetDualbound(model.get_scip_ptr());
	default:
		return decltype(SCIPgetDualbound(nullptr)){0};
	}
}

void DualBound::before_reset(scip::Model& /*unused*/) {
//	dual_bound_value = +/- Infinity;
    dual_bound_value = 0.0;
}

Reward DualBound::extract(scip::Model& model, bool /* done */) {
	auto dual_bound_value = dual_bound(model);
	return static_cast<double>(dual_bound_value);
}

}  // namespace ecole::reward

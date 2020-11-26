#include "ecole/observation/constraint-matrix.hpp"

namespace ecole::observation {

namespace {

auto compute_matrix(scip::Model const& model) -> utility::coo_matrix<double> {}

}  // namespace

void ConstraintMatrix::before_reset(scip::Model& /* model */) {
	on_reset = true;
}

auto ConstraintMatrix::extract(scip::Model& model, bool /* done */) -> utility::coo_matrix<double> {
	if (on_reset) {
		constraint_matrix = compute_matrix(model);
		on_reset = false;
	}
	return constraint_matrix;
}

}  // namespace ecole::observation

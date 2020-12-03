#include "ecole/observation/constraint-matrix.hpp"
#include "ecole/scip/model.hpp"
#include "ecole/scip/row.hpp"

namespace ecole::observation {

namespace {

using value_type = typename ConstraintMatrix::value_type;

/**
 * Number of non zero element in the constraint matrix.
 *
 * Row are counted once per right hand side and once per left hand side.
 */
auto matrix_nnz(scip::Model const& model) -> std::size_t {
	auto* const scip = model.get_scip_ptr();
	std::size_t nnz = 0;
	for (auto* row : model.lp_rows()) {
		auto const row_size = static_cast<std::size_t>(SCIProwGetNLPNonz(row));
		if (scip::get_unshifted_lhs(scip, row).has_value()) {
			nnz += row_size;
		}
		if (scip::get_unshifted_rhs(scip, row).has_value()) {
			nnz += row_size;
		}
	}
	return nnz;
}

auto compute_matrix(scip::Model const& model) -> utility::coo_matrix<value_type> {
	auto* const scip = model.get_scip_ptr();

	auto const nnz = matrix_nnz(model);
	auto values = decltype(utility::coo_matrix<value_type>::values)::from_shape({nnz});
	auto indices = decltype(utility::coo_matrix<value_type>::indices)::from_shape({2, nnz});

	std::size_t row_idx = 0;
	std::size_t j = 0;
	for (auto* const row : model.lp_rows()) {
		auto* const row_cols = SCIProwGetCols(row);
		auto const* const row_vals = SCIProwGetVals(row);
		auto const row_nnz = static_cast<std::size_t>(SCIProwGetNLPNonz(row));
		if (scip::get_unshifted_lhs(scip, row).has_value()) {
			for (std::size_t k = 0; k < row_nnz; ++k) {
				indices(0, j + k) = row_idx;
				indices(1, j + k) = static_cast<std::size_t>(SCIPcolGetLPPos(row_cols[k]));
				values[j + k] = -row_vals[k];
			}
			j += row_nnz;
			row_idx++;
		}
		if (scip::get_unshifted_rhs(scip, row).has_value()) {
			for (std::size_t k = 0; k < row_nnz; ++k) {
				indices(0, j + k) = row_idx;
				indices(1, j + k) = static_cast<std::size_t>(SCIPcolGetLPPos(row_cols[k]));
				values[j + k] = row_vals[k];
			}
			j += row_nnz;
			row_idx++;
		}
	}

	auto const n_cols = static_cast<std::size_t>(SCIPgetNLPCols(scip));
	return {values, indices, {row_idx, n_cols}};
}

}  // namespace

void ConstraintMatrix::before_reset(scip::Model& /* model */) {
	on_reset = true;
}

auto ConstraintMatrix::extract(scip::Model& model, bool /* done */) -> utility::coo_matrix<value_type> {
	if (on_reset) {
		constraint_matrix = compute_matrix(model);
		on_reset = false;
	}
	return constraint_matrix;
}

}  // namespace ecole::observation

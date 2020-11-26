#pragma once

#include <xtensor/xtensor.hpp>

#include "ecole/observation/abstract.hpp"
#include "ecole/utility/sparse-matrix.hpp"

namespace ecole::observation {

class ConstraintMatrix : public ObservationFunction<utility::coo_matrix<double>> {
public:
	void before_reset(scip::Model& model) override;

	auto extract(scip::Model& model, bool done) -> utility::coo_matrix<double> override;

private:
	utility::coo_matrix<double> constraint_matrix;
	bool on_reset = true;
};

}  // namespace ecole::observation

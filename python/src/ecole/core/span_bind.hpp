#include <utility>

#include <pybind11/pybind11.h>

namespace ecole {

/**
 * Bind a @ref std::span like view container to Python.
 */
template <typename Span, typename... Args> auto bind_span(pybind11::handle scope, Args&&... args) {
	namespace py = pybind11;
	using value_type = typename Span::value_type;
	using size_type = typename Span::size_type;
	using diff_type = typename Span::difference_type;
	using iter_type = typename Span::iterator;

	auto wrap_index = [](diff_type i, size_type n) {
		if (i < 0) {
			i += static_cast<diff_type>(n);
		}
		if (i < 0 || static_cast<size_type>(i) >= n) {
			throw py::index_error();
		}
		return static_cast<size_type>(i);
	};

	return py::class_<Span>(scope, std::forward<Args>(args)...)
		.def(
			"__getitem__",
			[wrap_index](Span const& sp, diff_type i) -> value_type& { return sp[wrap_index(i, sp.size())]; },
			py::return_value_policy::reference_internal  // ref + keepalive
			)
		.def(
			"__iter__",
			[](Span const& sp) {
				return py::make_iterator<py::return_value_policy::reference_internal, iter_type, iter_type, value_type&>(
					sp.begin(), sp.end());
			},
			py::keep_alive<0, 1>() /* Essential: keep span alive while iterator exists */
			)
		.def("__len__", &Span::size);
}

}  // namespace ecole

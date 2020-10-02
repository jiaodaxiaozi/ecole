cmake_minimum_required(VERSION 3.5)

# Temporary override pybind11 version to 2.5 otherwise pinned down by xtensor-python
# Until https://github.com/conda-forge/xtensor-python-feedstock/pull/65 is merged
include(FetchContent)
FetchContent_Declare(
	PyBind11
	GIT_REPOSITORY "https://github.com/pybind/pybind11"
	GIT_TAG "v2.5.0"
)
set(PYBIND11_CPP_STANDARD -std=c++17)
FetchContent_MakeAvailable(PyBind11)
# find_package(pybind11 REQUIRED)
find_package(xtensor REQUIRED)
find_package(xtensor-python REQUIRED)

pybind11_add_module(
	ecole-python MODULE SYSTEM
	src/ecole/core/core.cpp
	src/ecole/core/scip.cpp
	src/ecole/core/observation.cpp
	src/ecole/core/reward.cpp
	src/ecole/core/dynamics.cpp
)

target_include_directories(ecole-python PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/ecole/core)

# From Scikit-build
find_package(NumPy REQUIRED)
target_include_directories(ecole-python SYSTEM PRIVATE "${NumPy_INCLUDE_DIR}")

target_link_libraries(
	ecole-python
	PRIVATE
		Ecole::libecole
		Ecole::warnings
		xtensor
		xtensor-python
)

set_target_properties(ecole-python PROPERTIES OUTPUT_NAME core)

install(TARGETS ecole-python DESTINATION ecole)

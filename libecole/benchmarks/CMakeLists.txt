add_executable(
	benchmark-libecole
	src/main.cpp
	src/scip/bench-model.cpp
	src/bench-observation.cpp
)

target_compile_definitions(
	benchmark-libecole
	PRIVATE
		CATCH_CONFIG_ENABLE_BENCHMARKING
		TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../tests/data"
)

target_include_directories(benchmark-libecole PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

conan_cmake_run(
	CONANFILE conanfile.txt
	BASIC_SETUP
	CMAKE_TARGETS
	NO_OUTPUT_DIRS
	KEEP_RPATHS
	SKIP_STD
	BUILD missing
	OUTPUT_QUIET
)

target_link_libraries(
	benchmark-libecole
	PRIVATE
		Ecole::libecole
		Ecole::warnings
		Ecole::sanitizers
		CONAN_PKG::benchmark
)

set_target_properties(benchmark-libecole PROPERTIES
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN ON
)

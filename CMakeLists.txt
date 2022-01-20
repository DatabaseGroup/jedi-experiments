cmake_minimum_required(VERSION 3.7)

# Create the project.
project(jedi-similarity-lookup)

# Copied from https://stackoverflow.com/questions/14933172/how-can-i-add-a-minimum-compiler-version-requisite/14934542#14934542
# Use "export CXX=clang++-3.9" to point cmake to use clang.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # require at least gcc 7.0
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
        message(FATAL_ERROR "GCC version must be at least 7.0!")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # require at least clang 3.9
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.9)
        message(FATAL_ERROR "Clang version must be at least 3.9!")
    endif()
else()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with Clang and GCC.")
endif()

# Compiler flags.
# MUST be declared after project().
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -O3 -Wall -Wextra -Wpedantic -Wstrict-overflow=5")

# Verify which timing implementation to use.
# NOTE: Comes from timing-statistics.
include(CheckSymbolExists)
CHECK_SYMBOL_EXISTS(clock_gettime "time.h" HAVE_CLOCK_GETTIME)
if(NOT HAVE_CLOCK_GETTIME)
  add_definitions(-DGETRUSAGE)
endif()

# Build executable with the experiments.
add_executable(
  exp-lookup        # EXECUTABLE NAME
  src/exp-lookup.cc # EXECUTABLE SOURCE
)

# Link the timing library and header file.
add_library(
  TimingLibrary                              # LIBRARY NAME
  external/timing-statistics/timings/timing.cxx    # LIBRARY SOURCE
)
target_link_libraries(
  exp-lookup    # TARGET EXECUTABLE NAME
  TimingLibrary # LIBRARY NAME
)
target_include_directories(
  exp-lookup                          # TARGET EXECUTABLE NAME
  PUBLIC external/timing-statistics/timings # HEADER FILE
)

# Create header-only library 'TreeSimilarity' with our algorithms.
add_library(
  TreeSimilarity INTERFACE
)
target_include_directories(
  TreeSimilarity INTERFACE
  external/tree-similarity/src/parser
  external/tree-similarity/src/node
  external/tree-similarity/src/label
  external/tree-similarity/src/cost_model
  external/tree-similarity/src/join
  external/tree-similarity/src/ted_lb
  external/tree-similarity/src/ted_ub
  external/tree-similarity/src/ted
  external/tree-similarity/src/data_structures
  external/tree-similarity/src/lookup
  external/tree-similarity/src/lookup/scan
  external/tree-similarity/src/lookup/index
  external/tree-similarity/src/json
)

# Let the compiler know to find the header files in TreeSimilarity library.
target_link_libraries(
  exp-lookup     # EXECUTABLE NAME
  TreeSimilarity # LIBRARY NAME
)




























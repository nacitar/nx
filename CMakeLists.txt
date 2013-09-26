cmake_minimum_required(VERSION 2.6)
project (nx_project CXX)

include ("${CMAKE_CURRENT_LIST_DIR}/cmake/template.cmake")


# GoogleTest
# Using gtest just requires the include directory but building gtest requires
# the root directory too, so gtest-all.cc can include other .cc files.
# Requires pthread on linux
# TODO: get the root in a better way
Root(gtest_root_dir "../../googletest-read-only")
Relative(gtest_include_dir "${gtest_root_dir}" "include")
Relative(lib_sources "${gtest_root_dir}" "src/gtest-all.cc")
Relative(main_sources "${gtest_root_dir}" "src/gtest_main.cc")
SetIncludeDirectories(${gtest_include_dir} ${gtest_root_dir})
# lib itself
add_library(gtest ${lib_sources})
if (UNIX)
  target_link_libraries(gtest pthread)
endif(UNIX)
# driver for test applications
add_library(gtest_main ${main_sources})
target_link_libraries(gtest_main gtest)


# NX
# Requires rt on linux
Root(nx_include_dir "include")
Root(lib_sources "src/nx-all.cc")
Root(main_sources "src/nx_main.cc")
SetIncludeDirectories(${nx_include_dir})
# lib itself
add_library(nx ${lib_sources})
if (UNIX)
  target_link_libraries(nx rt)
endif(UNIX)
# driver for user applications
add_library(nx_main ${main_sources})
target_link_libraries(nx_main nx)


# NX Samples
SetIncludeDirectories(${nx_include_dir})
# - Hello World
Root(sources "samples/hello_world/main.cc")
add_executable(hello_world ${sources})
target_link_libraries(hello_world nx_main)


# NX Unit Tests
SetIncludeDirectories(${nx_include_dir} ${gtest_include_dir})
# - Digits
Root(sources "test/digits_unittest.cc")
add_executable(digits_unittest ${sources})
target_link_libraries(digits_unittest nx gtest_main)

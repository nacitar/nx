cmake_minimum_required(VERSION 2.6)
project (nx_project CXX)

include ("${CMAKE_CURRENT_LIST_DIR}/cmake/template.cmake")


# GoogleTest
# Using gtest just requires the include directory but building gtest requires
# the root directory too, so gtest-all.cc can include other .cc files.
# Requires pthread on linux
# TODO: get the root in a better way
Root(gtest_root_dir "../../googletest-read-only")
Relative(gtest_main_cc "${gtest_root_dir}" "src/gtest_main.cc")
Relative(gtest_include_dir "${gtest_root_dir}" "include")
Relative(sources "${gtest_root_dir}" "src/gtest-all.cc")
SetIncludeDirectories(${gtest_include_dir} ${gtest_root_dir})
add_library(gtest ${sources})
if (UNIX)
  target_link_libraries(gtest pthread)
endif(UNIX)


# NX
# Requires rt on linux
Root(nx_main_cc "src/nx_main.cc")
Root(nx_include_dir "include")
Root(sources "src/nx-all.cc")
SetIncludeDirectories(${nx_include_dir})
add_library(nx ${sources})
if (UNIX)
  target_link_libraries(nx rt)
endif(UNIX)


# NX Samples
SetIncludeDirectories(${nx_include_dir})
# - Working
Root(sources "samples/working/main.cc")
add_executable(working ${sources} ${nx_main_cc})
target_link_libraries(working nx)


# NX Unit Tests
SetIncludeDirectories(${nx_include_dir} ${gtest_include_dir})
# - Digits
Root(sources "test/digits_unittest.cc")
add_executable(digits_unittest ${sources} ${gtest_main_cc})
target_link_libraries(digits_unittest nx gtest)

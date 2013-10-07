########################################################################
# CMake build script for NX.

if(CMAKE_COMPILER_IS_GNUCXX)
  option(STATIC_RUNTIME "Statically link libstdc++ and libgcc" OFF)
endif()

########################################################################
#
# Project-wide settings

project (nx CXX)
cmake_minimum_required(VERSION 2.6.2)
enable_testing()

# Appends values to a space-delimited string
macro(Append var)
  foreach(value ${ARGN})
    set("${var}" "${${var}} ${value}")
  endforeach()
endmacro()

# Creates a binary name from a given target
macro(ExecutableName var target)
  set("${var}" "${CMAKE_EXECUTABLE_PREFIX}${target}${CMAKE_EXECUTABLE_SUFFIX}")
endmacro()

# Adds a test from the target name, using WINE if necessary
function(AddTest target)
  ExecutableName(target_binary "${target}")
  set(GTEST_XML_ARGS "--gtest_output=xml:${target}_results.xml")
  if (CMAKE_CROSSCOMPILING AND WIN32)
    add_test("${target}" wine "${target_binary}" "${GTEST_XML_ARGS}" ${ARGN})
  else()
    add_test("${target}" "${target_binary}" "${GTEST_XML_ARGS}" ${ARGN})
  endif()
endfunction()

# Compilation settings
if(CMAKE_COMPILER_IS_GNUCXX)
  set(flags "-O3 -DNDEBUG")
  set(CMAKE_C_FLAGS_RELEASE "${flags}")
  set(CMAKE_CXX_FLAGS_RELEASE "${flags}")

  set(flags "-O0 -fno-inline -DDEBUG")
  set(CMAKE_C_FLAGS_DEBUG "${flags}")
  set(CMAKE_CXX_FLAGS_DEBUG "${flags}")

  set(flags "-Wall -Wno-unused-function -Wno-unused-value -g3")
  Append(CMAKE_C_FLAGS "${flags}")
  Append(CMAKE_CXX_FLAGS "${flags} -std=c++11")
  if (STATIC_RUNTIME)
    set(flags "-static-libstdc++ -static-libgcc")
    Append(CMAKE_EXE_LINKER_FLAGS "${flags}")
    Append(CMAKE_SHARED_LINKER_FLAGS "${flags}")
    Append(CMAKE_MODULE_LINKER_FLAGS "${flags}")
  endif()
endif()

########################################################################
#
# Build 3rdparty libraries

# GoogleTest
add_subdirectory("3rdparty/googletest") # using its own CMakeLists.txt


########################################################################
#
# Defines the nx & nx libraries.  User projects should link with one of them.

# Maintained libraries/binaries
if(CMAKE_COMPILER_IS_GNUCXX)
  # Treat warnings as errors
  set(flags "-Werror")
  Append(CMAKE_C_FLAGS "${flags}")
  Append(CMAKE_CXX_FLAGS "${flags}")
endif()

include_directories("${nx_SOURCE_DIR}/include")

add_library(nx "src/nx-all.cc")
add_library(nx_main "src/nx_main.cc")
if (UNIX)
  target_link_libraries(nx rt)
endif()
target_link_libraries(nx_main nx)


########################################################################
#
# Samples on how to link applications with nx or nx_main

add_executable(hello_world "samples/hello_world/main.cc")
target_link_libraries(hello_world nx_main)

########################################################################
#
# NX Unit Tests

include_directories(SYSTEM "${gtest_SOURCE_DIR}/include")

add_executable(digits_unittest "test/digits_unittest.cc")
target_link_libraries(digits_unittest nx gtest_main)
AddTest(digits_unittest)

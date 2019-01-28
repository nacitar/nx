#
# Copyright (C) 2014 Jacob McIntosh <nacitar at ubercpp dot com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


########################################################################
#
# Project-wide settings

project (nx CXX)

# support feature detection; needs cmake 3.1+
cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

enable_testing()

include(external/cmake/build_utils.cmake)

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

# Projects rely upon: http://open-std.org/JTC1/SC22/WG21/docs/papers/2014/n4268.html
# for this reason, we need c++1z support and as a consequence can only
# currently use clang. =/
# See bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67991

# Compilation settings
if (TOOLCHAIN_CXX_GNU_COMPATIBLE)
  option(STATIC_RUNTIME "Statically link libstdc++ and libgcc" OFF)

  Append(C_FLAGS "-std=c99 -Wstrict-prototypes")
  Append(CXX_FLAGS "-std=c++17 -Wold-style-cast")
  Append(CC_FLAGS_RELEASE "-O3 -DNDEBUG")
  Append(CC_FLAGS_DEBUG "-g3 -O0 -fno-inline -DDEBUG")
  Append(CC_FLAGS "-Werror -Wall -Wno-unused-function -Wno-unused-value")
  Append(CC_FLAGS "-Wconversion -Wsign-conversion -funit-at-a-time")

  if (STATIC_RUNTIME)
    Append(LINK_FLAGS "-static-libstdc++ -static-libgcc")
  endif()
endif()

#get_property(cxx_known_features GLOBAL PROPERTY CMAKE_CXX_KNOWN_FEATURES)
#list(SORT cxx_known_features)
#message("Out of all possible C++ compiler features CMake knows about:")
#foreach(i ${cxx_known_features})
#  message("  ${i}")
#endforeach()
#message("")
#set(cxx_compile_features "${CMAKE_CXX_COMPILE_FEATURES}")
#list(SORT cxx_compile_features)
#message("your C++ compiler supports these features:")
#foreach(i ${cxx_compile_features})
#  message("  ${i}")
#endforeach()


########################################################################
#
# Build 3rdparty libraries

# GoogleTest
add_subdirectory("external/3rdparty/googletest")


########################################################################
#
# Build nx libraries

if (TARGET_WINDOWS)
  include_directories("${nx_SOURCE_DIR}/external/3rdparty/mingw-std-threads")
endif()

include_directories("${nx_SOURCE_DIR}/external/nx-core/include")
include_directories("${nx_SOURCE_DIR}/include")

# sources
ListSet(CXX_SOURCES
  "src/application.cc"
  "src/sigslot.cc"
  "src/string_util.cc"
	"src/handler.cc"
	"src/looper.cc"
	"src/message.cc")
AddLibrary(nx)

ListSet(CXX_SOURCES "src/nx_main.cc")
AddLibrary(nx_main)

if (TARGET_LINUX)
  target_link_libraries(nx rt pthread) # stdc++
endif()
target_link_libraries(nx_main nx)


########################################################################
#
# Samples on how to link applications with nx or nx_main

ListSet(CXX_SOURCES "samples/hello_world/main.cc")
AddExecutable(hello_world)
target_link_libraries(hello_world nx_main)

ListSet(CXX_SOURCES "samples/handler/main.cc")
AddExecutable(handler)
target_link_libraries(handler nx_main)

#ListSet(CXX_SOURCES "samples/sandbox/main.cc")
#AddExecutable(sandbox)
#target_link_libraries(sandbox nx_main)


########################################################################
#
# NX Unit Tests

include_directories(SYSTEM "${gtest_SOURCE_DIR}/include")

ListSet(CXX_SOURCES "test/digits_unittest.cc")
AddExecutable(digits_unittest)
target_link_libraries(digits_unittest nx gtest_main)
AddTest(digits_unittest)

ListSet(CXX_SOURCES "test/core_unittest.cc")
AddExecutable(core_unittest)
target_link_libraries(core_unittest nx gtest_main)
AddTest(core_unittest)

ListSet(CXX_SOURCES "test/sigslot_unittest.cc")
AddExecutable(sigslot_unittest)
target_link_libraries(sigslot_unittest nx gtest_main)
AddTest(sigslot_unittest)

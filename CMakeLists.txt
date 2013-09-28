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

# Utility macros
macro(Append var)
  foreach(value ${ARGN})
    set("${var}" "${${var}} ${value}")
  endforeach()
endmacro()
macro(AddLinkFlags)
  Append(CMAKE_EXE_LINKER_FLAGS ${ARGN})
  Append(CMAKE_SHARED_LINKER_FLAGS ${ARGN})
  Append(CMAKE_MODULE_LINKER_FLAGS ${ARGN})
endmacro()
macro(AddCXXFlags)
  Append(CMAKE_CXX_FLAGS ${ARGN})
endmacro()
macro(AddCFlags)
  Append(CMAKE_C_FLAGS ${ARGN})
endmacro()
macro(AddCCFlags)
  AddCXXFlags(${ARGN})
  AddCFlags(${ARGN})
endmacro()

# Compilation settings
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
  set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -DDEBUG")
  set(CMAKE_C_FLAGS_DEBUG "-O0 -fno-inline -DDEBUG")

  Append(CMAKE_CXX_FLAGS -std=c++11)
  AddCCFlags(-Wall -Wno-unused-function -Wno-unused-value -g3)
  if (STATIC_RUNTIME)
    AddLinkFlags(-static-libstdc++ -static-libgcc)
  endif()
endif()

########################################################################
#
# Build 3rdparty libraries

# GoogleTest
add_subdirectory("3rdparty/googletest") # using its own CMakeLists.txt


# Maintained libraries/binaries
# Treat warning as errors
AddCCFlags(-Werror)

########################################################################
#
# Defines the nx & nx libraries.  User projects should link with one of them.

include_directories("${nx_SOURCE_DIR}/include")

add_library(nx "src/nx-all.cc")
add_library(nx_main "src/nx_main.cc")
if (UNIX)
  target_link_libraries(nx rt)
endif(UNIX)
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

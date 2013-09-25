cmake_minimum_required(VERSION 2.6)
project (nx CXX C)

#
# --- FUNCTIONS
#

# Prepends the project's root path
function(Root destvar)
  set(result)
  foreach(value ${ARGN})
    list(APPEND result ${PROJECT_SOURCE_DIR}/${value})
  endforeach()
  set(${destvar} ${result} PARENT_SCOPE)
endfunction()

# String list operations
function(StrList op destvar)
  string(TOLOWER "${op}" op)
  if (op STREQUAL "append")
    set(result "${${destvar}}")
    foreach(value ${ARGN})
      set(result "${result} ${value}")
    endforeach()
    set(${destvar} "${result}" PARENT_SCOPE)
  endif()
endfunction()

# Adds flags to the linker for all modes of linking
function(AddLinkFlags)
  # Assign the flags
  StrList(APPEND CMAKE_EXE_LINKER_FLAGS ${ARGN})
  StrList(APPEND CMAKE_SHARED_LINKER_FLAGS ${ARGN})
  StrList(APPEND CMAKE_MODULE_LINKER_FLAGS ${ARGN})
  # Pass from our locals to our caller
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" PARENT_SCOPE)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}" PARENT_SCOPE)
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}" PARENT_SCOPE)
endfunction()

# Adds flags for both C and CXX
function(AddCCFlags)
  # Assign the flags
  StrList(APPEND CMAKE_C_FLAGS ${ARGN})
  StrList(APPEND CMAKE_CXX_FLAGS ${ARGN})
  # Pass from our locals to our caller
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" PARENT_SCOPE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
endfunction()

#
# --- Makefile
#

# Options
option(DEFINE_DEBUG "Build the project using debugging code" OFF)

# Debug build
if(DEFINE_DEBUG)
  message("Adding Debug flag...")
  add_definitions(-DDEBUG)
  set(CMAKE_BUILD_TYPE Debug)
  message("Build type is " ${CMAKE_BUILD_TYPE})
else()
  add_definitions(-DNDEBUG)
endif(DEFINE_DEBUG)


# Make gcc build as c++11 (VS10+ uses c++11 by default)
if(CMAKE_COMPILER_IS_GNUCXX)
  option(DEFINE_STATIC_RUNTIME "Statically link libstdc++ and libgcc" OFF)
  StrList(APPEND CMAKE_CXX_FLAGS -std=c++11)

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    AddCCFlags(-O0 -fno-inline)
  else()
    AddCCFlags(-O2)
  endif()
  AddCCFlags(-Wall -Wno-unused-function -Wno-unused-value -g3)
  if (DEFINE_STATIC_RUNTIME)
    AddLinkFlags(-static-libstdc++ -static-libgcc)
  endif()
endif()

# Where to put the libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)


# Add GoogleTest include directories
Root(include_dirs
 ../../googletest-read-only/include
 ../../googletest-read-only)
include_directories(${include_dirs})
# Build GoogleTest library
Root(source_files
  ../../googletest-read-only/src/gtest-all.cc)
add_library(gtest ${source_files})
# On linux it requires pthread
if (UNIX)
  target_link_libraries(gtest pthread)
endif(UNIX)


# Add nx include directories
Root(include_dirs
  include/main)
include_directories(${include_dirs})
# Build nx library 
Root(source_files
  src/main/nx-all.cc)
add_library(nx ${source_files})
# On linux it requires rt
if (UNIX)
  target_link_libraries(nx rt)
endif(UNIX)


# Build the working project
Root(source_files
  src/main/nx_main.cc
  projects/working/main.cc)
add_executable(working ${source_files})
# Depends upon nx and gtest
target_link_libraries(working nx gtest)

# Build the unit test
Root(source_files
  src/test/digits_unittest.cc
  projects/working/main.cc)
add_executable(digits_unittest
  ${source_files}
  ../../googletest-read-only/src/gtest_main.cc)
# Depends upon nx and gtest
target_link_libraries(digits_unittest nx gtest)

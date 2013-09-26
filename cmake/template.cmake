include ("${CMAKE_CURRENT_LIST_DIR}/util.cmake")

# Options
option(DEFINE_DEBUG "Build the project using debugging code" OFF)

# Debug build
if(DEFINE_DEBUG)
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


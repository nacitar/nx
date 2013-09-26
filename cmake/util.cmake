
# Prefixes a string to each item passed
function(PrefixList destvar prefix)
  set(result)
  foreach(value ${ARGN})
    list(APPEND result "${prefix}${value}")
  endforeach()
  set(${destvar} "${result}" PARENT_SCOPE)
endfunction()

# Prepends specified path
function(Relative destvar basepath)
  set(result)
  PrefixList(result "${basepath}/" ${ARGN})
  set(${destvar} "${result}" PARENT_SCOPE)
endfunction()

# Prepends the project's root path
function(Root destvar)
  set(result)
  Relative(result "${PROJECT_SOURCE_DIR}" ${ARGN})
  set(${destvar} "${result}" PARENT_SCOPE)
endfunction()

# String list operations (space delimited)
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

# Sets the include directories with the specified ones
function(SetIncludeDirectories)
  set_directory_properties(PROPERTIES INCLUDE_DIRECTORIES "")
  include_directories(${ARGN})
endfunction()


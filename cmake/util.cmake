# Appends values to a space-delimited string
macro(Append var)
  foreach(value ${ARGN})
    set("${var}" "${${var}} ${value}")
  endforeach()
endmacro()

# Appends CXX Flags
macro(AppendCXXFlags)
  Append(CMAKE_CXX_FLAGS ${ARGN})
endmacro()

# Appends C Flags
macro(AppendCFlags)
  Append(CMAKE_C_FLAGS ${ARGN})
endmacro()

# Appends flags to both CXX and C flags
macro(AppendCCFlags)
  AppendCXXFlags(${ARGN})
  AppendCFlags(${ARGN})
endmacro()

# Appends link flags (of all varieties)
macro(AppendLinkFlags)
  Append(CMAKE_EXE_LINKER_FLAGS ${ARGN})
  Append(CMAKE_SHARED_LINKER_FLAGS ${ARGN})
  Append(CMAKE_MODULE_LINKER_FLAGS ${ARGN})
endmacro()

# Creates a binary name from a given target
macro(ExecutableName var target)
  set("${var}" "${CMAKE_EXECUTABLE_PREFIX}${target}${CMAKE_EXECUTABLE_SUFFIX}")
endmacro()

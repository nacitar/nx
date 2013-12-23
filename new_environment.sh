#!/bin/bash

#
# Copyright (C) 2013 Jacob McIntosh <nacitar at ubercpp dot com>
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

cd "$(dirname "$0")"

function die() {
  echo "$@" 1>&2
  exit 1
}

# defaults
debug=0
cross=0
dest_dir=""
use_clang=0

# argument handling
for arg in "$@"
do
  if [ "$arg" == "--clang" ]; then
    use_clang=1
  elif [ "$arg" == "--cross" ]; then
    cross=1
  elif [ "$arg" == "--debug" ]; then
    debug=1
  elif [ "${arg:0:1}" == "-" ]; then
    die "Unknown argument $arg"
  elif [ -z "$dest_dir" ]; then
    cwd_real_path="$(readlink -f .)"
    dest_real_path="$(readlink -f "$arg")"
    if [ "${dest_real_path:0:${#cwd_real_path}}" != "$cwd_real_path" ]; then
      die "Build directory must be a child of the CWD, for safety."
    fi
    dest_dir="$arg"
  else
    die "Multiple destination directories found."
  fi
done

# default
if [ -z "$dest_dir" ]; then
  dest_dir="build"
fi

# flag preparation
cmake_flags=""
clang_flags=""
if [ "$debug" -ne 0 ]; then
  cmake_flags="$cmake_flags -DCMAKE_BUILD_TYPE=Debug"
else
  cmake_flags="$cmake_flags -DCMAKE_BUILD_TYPE=Release"
fi

if [ "$cross" -ne 0 ]; then
  toolchain="-DCMAKE_TOOLCHAIN_FILE=../cmake/mingw32_toolchain.cmake"
  cmake_flags="$cmake_flags $toolchain -DSTATIC_RUNTIME=1"
fi

# Make a new build environment
[ -d "$dest_dir" ] && rm -rf "$dest_dir"
if mkdir "$dest_dir" && pushd "$dest_dir"; then
  echo "Attempting to create build environment."
  if [ "$use_clang" -ne 0 ]; then
    CC=clang CXX=clang++ cmake $cmake_flags ..
  else
    cmake $cmake_flags ..
  fi
  popd  # build
  echo
else
  die "Failed to setup build environment in $dest_dir"
fi


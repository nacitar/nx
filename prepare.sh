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

root_dir="$(pwd)"
log_file_name=".prepare.log"
log_file="$root_dir/$log_file_name"
gtest_svn="http://googletest.googlecode.com/svn/trunk/"
function die() {
  echo "$@" 1>&2
  exit 1
}
function log() {
  echo "$@" &>> "$log_file"
}
function enter_dir() {
  pushd "$@" &>> "$log_file"
}
function exit_dir() {
  popd &>> "$log_file"
}

# Clear the log
echo -n "" > "$log_file"

# Get google test
enter_dir 3rdparty || die "Missing 3rdparty directory; this shouldn't happen!"
gtest_dir="googletest"
if enter_dir "$gtest_dir"; then
  log "Attempting to update googletest:"
  svn update &>> "$log_file" || log "Error updating googletest; ignoring."
  gtest_operation="Updated"
  exit_dir  # $gtest_dir
else
  log "Attempting to check out googletest:"
  svn checkout "$gtest_svn" "$gtest_dir" &>> "$log_file" ||
    die "Failed to fetch googletest."
  gtest_operation="Checked out"
fi
exit_dir  # 3rdparty
log

# Make a brand new native build environment
[ -d build ] && rm -rf build
if mkdir build && enter_dir build; then
  log "Attempting to create native build environment."
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    .. &>> "$log_file"
  exit_dir  # build
  log
else
  die "Failed to setup native  build environment in build/"
fi

# Make a brand new windows cross build environment
[ -d winbuild ] && rm -rf winbuild
if mkdir winbuild && enter_dir winbuild; then
  log "Attempting to create cross build environment."
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-mingw32.cmake \
    -DSTATIC_RUNTIME=1 \
    .. &>> "$log_file"
  exit_dir  # winbuild
else
  die "Failed to setup cross build environment in winbuild/"
fi

# Summary
echo "Operations:"
echo "- Log in $log_file_name"
echo "- $gtest_operation googletest in 3rdparty/$gtest_dir"
echo "- Prepared native environment in build/"
echo "- Prepared cross environment in winbuild/"

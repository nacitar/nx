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

gtest_svn="http://googletest.googlecode.com/svn/trunk/"
gstyle_svn="http://google-styleguide.googlecode.com/svn/trunk/"
function die() {
  echo "$@" 1>&2
  exit 1
}

# Get 3rdparty libraries
pushd external/3rdparty || die "Missing 3rdparty directory; this shouldn't happen!"
# Get googletest
gtest_dir="googletest"
if pushd "$gtest_dir"; then
  echo "Attempting to update googletest:"
  svn update || echo "Error updating googletest; ignoring."
  gtest_operation="Updated"
  popd  # $gtest_dir
else
  echo "Attempting to check out googletest:"
  svn checkout "$gtest_svn" "$gtest_dir" || die "Failed to fetch googletest."
  gtest_operation="Checked out"
fi
echo
# Get google-styleguide
gstyle_dir="google-styleguide"
if pushd "$gstyle_dir"; then
  echo "Attempting to update google-styleguide:"
  svn update || echo "Error updating google-styleguide; ignoring."
  gstyle_operation="Updated"
  popd  # $gstyle_dir
else
  echo "Attempting to check out google-styleguide:"
  if svn checkout "$gstyle_svn" "$gstyle_dir"; then
    gstyle_operation="Checked out"
  else
    echo "Failed to fetch google-styleguide; ignoring."
    gstyle_operation="Failed to check out"
  fi
fi
popd  # 3rdparty

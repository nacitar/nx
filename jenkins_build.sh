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

# Running this builds the project very simple, passing "test" executes tests.

cd "$(dirname "$0")"

function die() {
  echo "$@" 1>&2
  exit 1
}

arg=""
[ "$1" == "test" ] && arg="test"

result=0
pushd build || die "Native build environment is not present in build/"
make $arg || result=1
popd

pushd clangbuild || die "Clang build environment is not present in clangbuild/"
make $arg || result=1
popd

echo

pushd winbuild || die "Cross build environment is not present in winbuild/"
make $arg || result=1
popd

exit $result

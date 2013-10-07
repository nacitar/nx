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

[ -d build ] && rm -rf build
[ -d winbuild ] && rm -rf winbuild

if mkdir build && pushd build; then
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    ..
  popd
fi

if mkdir winbuild && pushd winbuild; then
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-mingw32.cmake \
    -DSTATIC_RUNTIME=1 \
    ..
  popd
fi

echo
echo "PREPARED:"
echo "- Windows: winbuild/"
echo "- Linux: build/"

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

# This file checks all C++ source files for adherence to google-styleguide.

cd "$(dirname "$0")"

function die() {
  echo "$@" 1>&2
  exit 1
}

cpplint="external/3rdparty/google-styleguide/cpplint/cpplint.py"

[ -x "$cpplint" ] || die "ERROR: 3rdparty google-styleguide is required."

code_dirs="src/ test/ include/nx/ external/nx-core/include/nx"

find $code_dirs -type f -name "*.cc" -o -name "*.h" | sort | while read fn; do
  # submodules use their own roots
  if [[ "$fn" == "external/"* ]]; then
    rootarg="$(echo "$fn" | cut -d'/' -f1-2)"
  else
    rootarg="."
  fi
  "$cpplint" --filter=-build/c++11 --root="$rootarg" "$fn" 2>&1 \
      | grep -v "^Total errors found: 0$"
done

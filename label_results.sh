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

# takes a label, and a folder
function process_folder() {
  for file in "$2"/*_results.xml; do
    if [ -r "$file" ]; then
      sed -i -e's/classname="/classname="'"$1"'./g' "$file" ||
        echo "Error labeling file: $file" 1>&2
    fi
  done
}

# Native tests
process_folder "Native" build
# Windows tests; labeling as WINE because the tests are run via WINE
process_folder "Wine" winbuild


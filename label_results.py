#!/usr/bin/env python3

#
# Copyright (C) 2019 Jacob McIntosh <nacitar at ubercpp dot com>
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

import sys
if sys.version_info < (3, 6):
    raise RuntimeError('Python 3.6+ is required.')

import pathlib
import subprocess
import re
this_file = pathlib.Path(__file__).resolve()

_label_pattern = re.compile('(classname=")([^"]*)(")')
def main(argv = None):
    if argv is None:
        argv = sys.argv
    
    retval = 0
    for build_type in ['native', 'clang', 'mingw32']:  # no avr
        dir_path = pathlib.Path(this_file.parent, 'build', build_type)
        for path in dir_path.glob('*_results.xml'):
            new_lines = []

            with path.open('r') as f:
                while True:
                    line = f.readline()
                    if not line:
                        break
                    new_lines.append(
                            _label_pattern.sub(f'\\1{build_type}\\3', line))

            with path.open('w') as f:
                f.writelines(new_lines)

    return retval

if __name__ == '__main__':
    sys.exit(main())

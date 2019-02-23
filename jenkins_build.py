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

# Running this builds the project very simply, passing "test" executes tests.

import sys
if sys.version_info < (3, 6):
    raise RuntimeError('Python 3.6+ is required.')

import pathlib
import subprocess
import argparse
this_file = pathlib.Path(__file__).resolve()

def main(argv = None):
    if argv is None:
        argv = sys.argv
    parser = argparse.ArgumentParser(
            description='Build project for Jenkins', allow_abbrev=False)
    parser.add_argument('--test', action='store_true')
    args = parser.parse_args(argv[1:])

    retval = 0
    cmd = ['make']
    if args.test:
        cmd.append('test')
    for build_type in ['native', 'clang', 'mingw32']:  # no avr
        if subprocess.call(cmd, cwd=pathlib.Path(
                this_file.parent, 'build', build_type)):
            retval += 1

    return retval

if __name__ == '__main__':
    sys.exit(main())

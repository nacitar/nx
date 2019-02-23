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

# This file checks all C++ source files for adherence to google-styleguide.

import sys
if sys.version_info < (3, 6):
    raise RuntimeError('Python 3.6+ is required.')

import pathlib
import os
this_file = pathlib.Path(__file__).resolve()

import importlib.util
cpplint_path = pathlib.Path(
        this_file.parent, 'external/3rdparty/cpplint/cpplint.py')
cpplint_spec = importlib.util.spec_from_file_location('cpplint', cpplint_path)
cpplint = importlib.util.module_from_spec(cpplint_spec)
cpplint_spec.loader.exec_module(cpplint)

def listize(obj):
    """ If obj is iterable and not a string, returns a new list with the same
    contents.  Otherwise, returns a new list with obj as its only element.
    """
    if not isinstance(obj, str):
        try:
            return list(obj)
        except:
            pass
    return [obj]

def iter_children_with_extension(root_dirs, extension_list):
    root_dirs = listize(root_dirs)
    print(repr(root_dirs))
    extension_list = [x.lower() for x in listize(extension_list)]
    for path in root_dirs:
        for root, dirs, files in os.walk(path):
            for filename in files:
                if pathlib.Path(filename).suffix.lower() in extension_list:
                    yield pathlib.Path(root, filename)
code_dirs = [
    'src/',
    'test/',
    'include/nx/',
    'external/nx-core/include/nx'
]

def main():
    retval = 0
    for full_path in iter_children_with_extension(
            [pathlib.Path(this_file.parent, code_dir)
                for code_dir in code_dirs],
            ['.cc', '.h']):
        if full_path.parts[len(this_file.parts)-1] == 'external':
            cpplint_repository = pathlib.Path(
                    *full_path.parts[:len(this_file.parts)+1])
        else:
            cpplint_repository = this_file.parent
        try:
            original_argv = sys.argv
            sys.argv = [
                    str(cpplint_path),
                    '--quiet',
                    '--filter=-build/c++11',
                    f'--repository={cpplint_repository}',
                    str(full_path)
            ]
            print(repr(sys.argv))
            cpplint.main()
        except SystemExit as exit:
            if exit.code != 0:
                retval += 1
        finally:
            sys.argv = original_argv
    return retval

if __name__ == '__main__':
    sys.exit(main())

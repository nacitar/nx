#!/usr/bin/env python3

#python 3.6+ required

import inspect, os
this_dir = os.path.abspath(os.path.dirname(inspect.getfile(inspect.currentframe())))
cmake_repo_dir = os.path.join(this_dir, 'external', 'cmake')
import sys
import os
import re
import subprocess

import argparse
import shutil

_get_version_pattern = re.compile(r'\d+\.\d+\.\d+')
def get_version(path):
    # can't use dumpversion because clang pretends to be an old gcc
    if os.access(path, os.X_OK):
        output = subprocess.check_output(
                [path, "--version"]).decode(
                        sys.getdefaultencoding())
        parts = _get_version_pattern.search(output).group(0).split('.')
        return (int(parts[0])*10000 + int(parts[1])*100 + int(parts[2]))
    return 0

def get_versioned_binary(base_binary, min_version=0):
    # if the default doesn't meet our needs, find the highest we have
    binary_pattern=re.compile('{}{}'.format(re.escape(base_binary),r'-\d'))
    current_name = shutil.which(base_binary)
    if current_name:
        current_version = get_version(current_name)
        if current_version >= min_version:
            return current_name
    else:
        current_version = 0
    for path in os.getenv('PATH').split(os.path.pathsep):
        path = path.rstrip(os.path.sep)
        if os.path.isdir(path):
            for filename in os.listdir(path):
                if binary_pattern.match(filename):
                    full_name = os.path.join(path, filename)
                    version = get_version(full_name)
                    if version > current_version:
                        current_name = full_name
                        current_version = version
    if min_version > current_version:
        raise RuntimeError(
                f'{base_binary} Version: {current_version}, Minimum Required: {min_version}')
    return current_name


def main():
    parser = argparse.ArgumentParser(description='Create a new environment', allow_abbrev=False)
    compiler = parser.add_mutually_exclusive_group()
    compiler.add_argument('--clang', action='store_true')
    compiler.add_argument('--mingw32', action='store_true')
    compiler.add_argument('--avr', action='store_true')
    parser.add_argument('--debug', action='store_true')
    parser.add_argument('dest_dir', type=str)
    args = parser.parse_args()
    abs_cwd = os.path.abspath(os.path.curdir)
    abs_dest_dir = os.path.abspath(args.dest_dir)
    if not abs_dest_dir.startswith(abs_cwd) or abs_dest_dir == abs_cwd:
        raise RuntimeError('Build directory must be a child of the CWD, for safety.')

    c_compiler = 'gcc'
    cxx_compiler = 'g++'
    base_prefix = ''
    base_prefix_dash = ''
    min_version = 80100

    cmake_cmd = [ 'cmake' ]
    if args.debug:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Debug')
    else:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Release')

    # TODO: .exe?
    if args.mingw32:
        base_prefix='x86_64-w64-mingw32'
        rc_binary = shutil.which(f'{base_prefix}-windres')
        cmake_cmd.extend([
            '-DCMAKE_SYSTEM_NAME=Windows',
            f'-DCMAKE_RC_COMPILER={rc_binary}',
            '-DSTATIC_RUNTIME=1'])

    elif args.avr:
        base_prefix = "avr"
        objcopy_binary = shutil.which(f'{base_prefix}-objcopy')
        size_tool_binary = shutil.which(f'{base_prefix}-size')
        cmake_cmd.extend([
            '-DCMAKE_SYSTEM_NAME=Generic',
            f'-DCMAKE_OBJCOPY={objcopy_binary}',
            f'-DAVR_SIZE_TOOL={size_tool_binary}'])
    elif args.clang:
        c_compiler = 'clang'
        cxx_compiler = 'clang++'
        min_version = 50000
    if base_prefix:
        base_prefix_dash = f'{base_prefix}-'
    else:
        base_prefix_dash = ''

    cc_binary = get_versioned_binary(f'{base_prefix_dash}{c_compiler}', min_version)
    cxx_binary = get_versioned_binary(f'{base_prefix_dash}{cxx_compiler}', min_version)

    # TODO: recursively delete abs_dest_dir
    if not os.path.exists(abs_dest_dir):
        os.makedirs(abs_dest_dir)
    my_env = os.environ.copy()
    my_env['CC'] = cc_binary
    my_env['CXX'] = cxx_binary

    cmake_cmd.extend([
            f'-DCMAKE_C_COMPILER={cc_binary}',
            f'-DCMAKE_CXX_COMPILER={cxx_binary}',
            f'-DCMAKE_FIND_ROOT_PATH=/usr/{base_prefix}',
            '-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER',
            '-DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY',
            '-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY'])

    cmake_cmd.extend([this_dir])

    print(repr(cmake_cmd))
    sys.exit(subprocess.call(cmake_cmd, env=my_env, cwd=args.dest_dir))

if __name__== '__main__':
    main()


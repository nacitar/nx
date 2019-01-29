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

_gcc_version_pattern = re.compile('\d+\.\d+\.\d+')
def gcc_version(path):
    # can't use dumpversion because clang pretends to be an old gcc
    output = subprocess.check_output(
            [path, "--version"]).decode(
                    sys.getdefaultencoding())
    parts = _gcc_version_pattern.search(output).group(0).split('.')
    return (int(parts[0])*10000 + int(parts[1])*100 + int(parts[2]))

def get_gcc(base_gcc_binary, min_version=None):
    # if the default doesn't meet our needs, find the highest we have
    binary_pattern=re.compile(f'{re.escape(base_gcc_binary)}-\d')
    current_name = base_gcc_binary
    current_version = gcc_version(current_name)
    if min_version != None and current_version < min_version:
        for path in os.getenv('PATH').split(os.path.pathsep):
            path = path.rstrip(os.path.sep)
            if os.path.isdir(path):
                for filename in os.listdir(path):
                    full_path = f"{path}{os.path.sep}{filename}"
                    if (binary_pattern.match(filename) and os.access(
                            full_path, os.X_OK)):
                        version = gcc_version(filename)
                        if version > current_version:
                            current_name = filename
                            current_version = version
    return current_name


def main():
    parser = argparse.ArgumentParser(description='Create a new environment', allow_abbrev=False)
    compiler = parser.add_mutually_exclusive_group()
    compiler.add_argument('--clang', action='store_true')
    compiler.add_argument('--mingw32', action='store_true')
    compiler.add_argument('--avr', action='store_true')
    parser.add_argument('--debug', action='store_true')
    parser.add_argument('dest_dir', type=str, default='build', nargs='?')
    args = parser.parse_args()
    abs_cwd = os.path.abspath(os.path.curdir)
    abs_dest_dir = os.path.abspath(args.dest_dir)
    if not abs_dest_dir.startswith(abs_cwd):
        raise RuntimeError('Build directory must be a child of the CWD, for safety.')

    c_compiler = 'gcc'
    cxx_compiler = 'g++'
    base_prefix = ''
    min_version = 80100

    cmake_cmd = [ 'cmake' ]
    if args.debug:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Debug')
    else:
        cmake_cmd.append('-DCMAKE_BUILD_TYPE=Release')

    if args.mingw32:
        base_prefix="x86_64-w64-mingw32-"
        cmake_cmd.extend([
            f'-DCMAKE_TOOLCHAIN_FILE={cmake_repo_dir}/mingw32_toolchain.cmake',
            '-DSTATIC_RUNTIME=1'])
    elif args.avr:
        base_prefix = "avr-"
        cmake_cmd.append(f'-DCMAKE_TOOLCHAIN_FILE={cmake_repo_dir}/avr_toolchain.cmake')
    elif args.clang:
        c_compiler = 'clang'
        cxx_compiler = 'clang++'
        min_version = 50000

    cc_binary = get_gcc(f'{base_prefix}{c_compiler}', min_version)
    cxx_binary = get_gcc(f'{base_prefix}{cxx_compiler}', min_version)

    # TODO: recursively delete abs_dest_dir
    os.mkdir(abs_dest_dir)
    my_env = os.environ.copy()
    my_env['CC'] = cc_binary
    my_env['CXX'] = cxx_binary

    # undocumented cmake args to let me 
    #cmake_cmd.extend([f'-B{args.dest_dir}', f'-H{this_dir}'])
    cmake_cmd.extend([this_dir])

    print(repr(cmake_cmd))
    code = subprocess.call(cmake_cmd, env=my_env, cwd=args.dest_dir)
    print(f'Code: {code}')
    print(cc_binary)
    print(cxx_binary)

if __name__== '__main__':
    main()


nx
==

A library project to enhance C++.


License
=======

Where possible, the license has been placed
at the beginning of all files in this repository.  The license is as follows.

        Copyright (C) 2019 Jacob McIntosh <nacitar at ubercpp dot com>

        Licensed under the Apache License, Version 2.0 (the "License");
        you may not use this file except in compliance with the License.
        You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

        Unless required by applicable law or agreed to in writing, software
        distributed under the License is distributed on an "AS IS" BASIS,
        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
        See the License for the specific language governing permissions and
        limitations under the License.


Documentation
=============

Run "doxygen" in the repository root folder, and documentation will be
available in doc/html/index.html


Compilation
===========

Look at new\_environment.py if you wish to manually setup your build
environment.  Within the environment, building can be performed simply with
"make" and then "make test".  Otherwise, you can build it by executing the
"Build" commands listed under the Configuring Jenkins section below.


Configuring Jenkins
===================

Jenkins support currently only works for building on linux, and requires bash,
sed, git, svn, wine, and the github plugin for Jenkins.  Wine is required
specifically because the windows builds of this library are intended to
function under wine, and as such jenkins builds windows binaries and executes
their test cases under wine.

- Github Project

        http://github.com/nacitar/nx/

- Git repositories

        https://github.com/nacitar/nx.git

- Build - Execute Shell (for compilation)

        ./release_builds.py
        ./jenkins_build.py

- Build - Execute Shell (for test execution)

        ./jenkins_build.py --test
        ./label_results.py

- Post Build - Public JUnit Test Results Report

        build/native/*.xml, build/clang/*.xml, build/mingw32/*.xml


Gentoo Setup
============

        sudo crossdev --stable --target x86_64-w64-mingw32 --ex-gcc --ex-gdb
        sudo crossdev --stable --target cross-avr --ex-gcc --ex-gdb

        # package.accept_keywords
        #
        # need gcc-8.1.0+ to fix a bug
        # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67991
        =sys-devel/gcc-8.2.0-r6 ~amd64
        =cross-avr/gcc-8.2.0-r6 ~amd64
        =cross-x86_64-w64-mingw32/gcc-8.2.0-r6 ~amd64
        cross-x86_64-w64-mingw32/mingw64-runtime ~amd64


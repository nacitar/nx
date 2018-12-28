nx
==

A library project to enhance C++11.


License
=======

Where possible, the license has been placed
at the beginning of all files in this repository.  The license is as follows.

        Copyright (C) 2013 Jacob McIntosh <nacitar at ubercpp dot com>

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

Look at prepare.sh, particularly where it makes build environments, if you wish
to manually setup your build environment.  Within the environment, building can
be performed simply with "make" and then "make test".  Otherwise, you can build
it by executing the "Build" commands listed under the Configuring Jenkins
section below.


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

        ./prepare_3rdparty.sh
        ./release_builds.sh
        ./jenkins_build.sh

- Build - Execute Shell (for test execution)

        ./jenkins_build.sh test
        ./label_results.sh

- Post Build - Public JUnit Test Results Report

        build/*.xml, winbuild/*.xml


Gentoo Setup
============

        sudo crossdev --stable --target x86_64-w64-mingw32 --ex-gcc --ex-gdb



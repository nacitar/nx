nx
==

A library project to enhance C++11.


Compiling
=========

Look at prepare.sh, particularly where it makes build environments, if you wish
to manually setup your build environment.  Otherwise, you can build it by
executing the "Build" commands listed under the Configuring Jenkins section
below.

Configuring Jenkins
===================
Jenkins support currently only works for building on linux, and requires bash,
sed, git, svn, and the github plugin for Jenkins.

- Github Project

    http://github.com/nacitar/nx/

- Git repositories

    https://github.com/nacitar/nx.git

- Build - Execute Shell (for compilation)

    ./prepare.sh
    ./jenkins_build.sh

- Build - Execute Shell (for test execution)

    ./jenkins_build.sh test
    ./label_results.sh

- Post Build - Public JUnit Test Results Report

    build/*.xml, winbuild/*.xml



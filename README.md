nx
==

A library project to enhance C++11.


Compiling
=========

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

Cross-compiling for windows
===========================

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-mingw32.cmake -DDEFINE_STATIC_RUNTIME=1 ..
    make


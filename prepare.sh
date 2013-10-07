#!/bin/bash

if mkdir build && pushd build; then
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    ..
  popd
fi

if mkdir winbuild && pushd winbuild; then
  cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-mingw32.cmake \
    -DDEFINE_STATIC_RUNTIME=1 \
    ..
  popd
fi

echo
echo "PREPARED:"
echo "- Windows: winbuild/"
echo "- Linux: build/"

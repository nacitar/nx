#!/bin/bash

./new_environment.sh build/native
./new_environment.sh --clang build/clang
./new_environment.sh --mingw32 build/win
./new_environment.sh --avr build/avr

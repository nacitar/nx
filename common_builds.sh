#!/bin/bash

./new_environment.sh --debug build/native
./new_environment.sh --debug --clang build/clang
./new_environment.sh --debug --mingw32 build/win
./new_environment.sh --avr build/avr

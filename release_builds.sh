#!/bin/bash

./new_environment.py build/native
./new_environment.py --clang build/clang
./new_environment.py --mingw32 build/win
./new_environment.py --avr build/avr

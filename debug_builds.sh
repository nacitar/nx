#!/bin/bash

./new_environment.py --debug build/native
./new_environment.py --debug --clang build/clang
./new_environment.py --debug --mingw32 build/win
./new_environment.py --avr build/avr

#!/usr/bin/env python3

import new_environment

new_environment.main(['--debug', 'build/native'])
new_environment.main(['--debug', '--clang', 'build/clang'])
new_environment.main(['--debug', '--mingw32', 'build/mingw32'])
new_environment.main(['--debug', '--avr', 'build/avr'])

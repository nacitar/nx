#!/usr/bin/env python3

import new_environment

new_environment.main(['build/native'])
new_environment.main(['--clang', 'build/clang'])
new_environment.main(['--mingw32', 'build/mingw32'])
new_environment.main(['--avr', 'build/avr'])

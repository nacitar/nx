#!/bin/bash
cd "$(dirname "$0")"

[ -d "googletest" ] && rm -rf googletest

svn checkout http://googletest.googlecode.com/svn/trunk/ googletest

#!/bin/bash

cd $(dirname $(readlink -f $0))/..

find . -name '*.cpp' | grep -v rules | grep -v stuff | grep -v tab.cpp | grep -v .history | grep -v CMakeFiles| grep -v tab.cpp | grep -v lexer | grep -v .old | xargs clang-tidy -p ./build -header-filter=.*
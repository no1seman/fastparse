#!/bin/bash

cd $(dirname $(readlink -f $0))/..

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .
make
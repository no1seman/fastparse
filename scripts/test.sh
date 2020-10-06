#!/bin/bash

cd $(dirname $(readlink -f $0))/../test
tarantool test.lua

cd ./tmp
list=$(find . -name '*.json')

for json in $list; do
    python3 -m json.tool --sort-keys $json > $json'_'
    mv $json'_' $json
done
#!/bin/bash

cd $(dirname $(readlink -f $0))/../
rm -f -r ./static/*
pvs-studio-analyzer analyze --cfg ./scripts/PVS-Studio.cfg
plog-converter -t fullhtml -o ./static ./static/lualibgraphqlparser.log

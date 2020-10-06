#!/bin/bash

wget http://ftp.gnu.org/gnu/bison/bison-3.7.2.tar.gz
tar -xvf bison-3.7.2.tar.gz
cd bison-3.7.2
./configure
make
sudo make install
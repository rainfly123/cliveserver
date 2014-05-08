#!/bin/sh 
autoreconf
automake --add-missing
./configure
make

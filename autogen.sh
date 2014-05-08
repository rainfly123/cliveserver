#!/bin/sh 
autoreconf
automake --add-misssing
./configure
make

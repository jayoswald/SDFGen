#!/bin/bash

for ARG in "$@"; do 
    if [ "$ARG" == clean ]; then
        cd ../../src
        make clean && make -j || exit
        cd -
    fi    
done

../../src/SDFgen part_3_bulk.stl 0.01 10

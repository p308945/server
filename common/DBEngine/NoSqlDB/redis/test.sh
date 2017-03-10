#!/bin/bash
rm -rf test
g++ -I../../../util -x c++ -std=c++11 -Wall -g -Wno-deprecated -Wno-unused-function test.cpp.dbg -L./ -lredisclient -lpthread -o test

#!/bin/bash
#Build and run
echo "Building"
g++ minimal.cpp util.h `wx-config --cxxflags --libs std` `pkg-config --cflags --libs opencv` -o minimalcv -std=c++11
echo "Running"
gksudo ./minimalcv



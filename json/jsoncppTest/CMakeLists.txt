cmake_minimum_required(VERSION 2.8)
project( jtest )
include_directories(./)
add_executable( jtest jtest.cpp jsoncpp.cpp )

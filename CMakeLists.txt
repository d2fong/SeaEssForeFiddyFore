cmake_minimum_required(VERSION 3.2)
project(cs4544Reeal)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    test/ClientTest.cpp
    test/ClientTest.h
    Client.cpp
    Client.h
    client1.c)

add_executable(cs4544Reeal ${SOURCE_FILES})

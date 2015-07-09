cmake_minimum_required(VERSION 3.2)
project(cs4544Reeal)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    test/ClientTest.cpp
    test/ClientTest.h
    Client.cpp
    Client.h
    client1.c
    Binder.cpp
    binder_main.cpp)

#add_executable(cs4544Reeal ${SOURCE_FILES})
add_executable(binder binder_main.cpp Binder.cpp helpers.cpp Message.cpp DB.cpp)
add_executable(client CLient.cpp rpc_client.cpp helpers.cpp client1.c DB.cpp)


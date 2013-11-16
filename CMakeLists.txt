
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(RX621_Writer)

ADD_EXECUTABLE(rx621writer main.cpp serial.cpp rx621writer.cpp)

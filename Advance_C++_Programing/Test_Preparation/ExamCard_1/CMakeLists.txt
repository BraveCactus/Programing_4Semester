# 1.	Cmake. Написать Cmake для проекта, состоящего из main.cpp, class1.h, class1.cpp. Стандарт С++ - 17.
cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)

add_executable(MyProject main.cpp class1.h class1.cpp)
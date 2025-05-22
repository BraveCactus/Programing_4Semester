#2.	Cmake. Алгоритм сборки библиотеки без зависимостей из исходных кодов, которые содержат CmakeLists.txt.

# mkdir build
# cd build
# cmake ..
# cmake --build .


cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMALE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)

add_executable(MyProject main.cpp class1.h class1.cpp)

#CMakeLists.txt для библиотеки:
add_library(MyLib STATIC src1.cpp src2.cpp lib1.h)

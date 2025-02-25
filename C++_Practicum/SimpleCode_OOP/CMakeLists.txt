cmake_minimum_required(VERSION 3.10)

# Укажите имя проекта
project(MyProject)

# Укажите стандарт C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавьте исполняемый файл
add_executable(Lesson_2 source/Lesson_2.cpp)
add_executable(Lesson_3 source/Lesson_3.cpp)
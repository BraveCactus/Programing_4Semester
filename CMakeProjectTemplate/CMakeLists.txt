cmake_minimum_required(VERSION 3.10)

# Укажите имя проекта
project(MyProject)

# Укажите стандарт C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавьте путь к директории с заголовочными файлами
include_directories(include)

# Добавьте библиотеку
add_library(myLib2 source/my_libs/sum.cpp)

# Добавьте исполняемый файл
add_executable(MyExe source/main.cpp)


# Свяжите исполняемый файл с библиотекой
target_link_libraries(MyExe PRIVATE myLib2)

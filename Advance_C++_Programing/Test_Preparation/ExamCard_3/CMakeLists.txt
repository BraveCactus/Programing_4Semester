#3. Cmake. Добавление сторонней прекомпилированной библиотеки к своему проекту.

cmake_minimum_required(VERSION 3.11)
project(MyProject)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)

find_package(OpenCV REQUIRED)

set(OpenCV_INCLUDE_DIR /include)
set(OpenCV_LIBS_DIR /source)

# Пример добавления исполняемого файла (опционально)
add_executable(MyProject main.cpp)

# Указываем путь к заголовочным файлам
target_include_directories(MyProject PRIVATE ${OpenCV_INCLUDE_DIR})

target_link_libraries(MyProject PRIVATE ${OpenCV_LIBS_DIR})
cmake_minimum_required(Version 2.8)

project(example) # Название проекта

set(SOURCE_EXE main.cpp) # Установка переменной со списком исходников

set(SOURCE_LIB foo.cpp) # Установка переменной для библиотеки

add_library(foo STATIC ${SOURCE_LIB}) #Создание статической библиотеки с именем foo

add_executable(main ${SOURCE_EXE}) #Создает исполняемый файл с именем main

target_link_libraries(main foo) 
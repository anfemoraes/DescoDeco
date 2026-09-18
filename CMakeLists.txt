cmake_minimum_required(VERSION 3.16)

project(DescoDeco
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(DescoDeco
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
)

target_link_libraries(DescoDeco PRIVATE
    Qt6::Widgets
)
cmake_minimum_required(VERSION 3.16)

project(DescoDeco
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBZIP REQUIRED libzip)

qt_standard_project_setup()

qt_add_executable(DescoDeco
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/ZipExtractor.cpp
    src/ZipExtractor.h
)

target_include_directories(DescoDeco PRIVATE
    ${LIBZIP_INCLUDE_DIRS}
)

target_link_libraries(DescoDeco PRIVATE
    Qt6::Widgets
    ${LIBZIP_LIBRARIES}
)
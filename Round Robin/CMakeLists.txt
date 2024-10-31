cmake_minimum_required(VERSION 3.29)
project(RoundRobin)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS
        Core
        Quick
        REQUIRED)

add_executable(${PROJECT_NAME}
        main.cpp
)

target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Quick
)


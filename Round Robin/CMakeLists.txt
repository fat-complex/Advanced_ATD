cmake_minimum_required(VERSION 3.29)
project(RoundRobin)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QML2_IMPORT_PATH "/home/user/Qt5.6.3/5.6.3/gcc_64/qml")

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Quick
        Qml
        Gui
        REQUIRED)

set(SOURCES main.cpp
        RoundRobin.cpp
        RoundRobin.h)
qt6_add_resources(SOURCES qml.qrc)

add_executable(${PROJECT_NAME}
        ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
        Qt6::Core
        Qt6::Quick
        Qt6::Qml
        Qt6::Gui
)


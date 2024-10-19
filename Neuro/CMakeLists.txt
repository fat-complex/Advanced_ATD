cmake_minimum_required(VERSION 3.29)
project(Neural)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
        main.cpp
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/samples
            ${CMAKE_BINARY_DIR}/samples
        COMMENT "Copying samples into binary dir"
)


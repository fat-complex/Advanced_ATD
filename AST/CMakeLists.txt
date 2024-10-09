cmake_minimum_required(VERSION 3.29)
project(Advanced_ATD)

set(CMAKE_CXX_STANDARD 20)

add_executable(Advanced_ATD
        main.cpp
        parenthesis_substitution.cpp
        parenthesis_substitution.h
        simple_tree.h
        tokenizer.h
        tokenizer.cpp)

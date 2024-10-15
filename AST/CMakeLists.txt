cmake_minimum_required(VERSION 3.29)
project(AST)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME}
        main.cpp
        parenthesis_substitution.cpp
        parenthesis_substitution.h
        simple_tree.h
        tokenizer.h
        tokenizer.cpp
        make_ast.h
        make_ast.cpp)

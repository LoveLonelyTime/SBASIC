cmake_minimum_required(VERSION 2.8)

project(SBASIC)
set(DIR_SRCS main.cpp analyzer.cpp language.cpp lexer.cpp)
add_executable(${PROJECT_NAME} ${DIR_SRCS})
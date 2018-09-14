cmake_minimum_required(VERSION 3.12)
project(ponto C)

set(CMAKE_C_STANDARD 99)

add_executable(ponto main.c libfprint-0.7.0/examples/cadastro.c libfprint-0.7.0/examples/main.c)
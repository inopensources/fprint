cmake_minimum_required(VERSION 3.12)
project(fprint-infarma)


set(SOURCE_FILES libfprint-0.7.0/examples/ponto.c)
add_executable(ponto.c main.c libfprint-0.7.0/examples/cadastro.c libfprint-0.7.0/examples/main.c)
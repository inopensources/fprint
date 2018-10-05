cmake_minimum_required(VERSION 3.12)
project(untitled C)

set(CMAKE_C_STANDARD 99)

add_executable(ponto_infarma verify_live.c decode.c encode.c structs.h utils.h remote_database.c remote_database.h)

target_link_libraries(ponto_infarma fprint)
target_link_libraries(ponto_infarma curl)
target_link_libraries(ponto_infarma websockets)
target_link_libraries(ponto_infarma json-c)

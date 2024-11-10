cmake_minimum_required(VERSION 3.29)
project(nagyHazi C)

set(CMAKE_C_STANDARD 11)

add_executable(nagyHazi main.c debugmalloc.h
        datum.h
        ido.h
        esemeny.h
        adatbazis.h
        seged.h
        esemeny_kezelo.h)
target_compile_options(nagyHazi PUBLIC -Wall -Werror)
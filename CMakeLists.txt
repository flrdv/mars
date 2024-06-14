cmake_minimum_required(VERSION 3.28)
project(mars C)

set(CMAKE_C_STANDARD 11)
include_directories(
        src
)

add_executable(mars main.c
        src/net/http/parser.c
        src/net/http/parser.h
        src/net/http/core.c
        src/net/http/core.h
        src/net/http/status.h
        src/misc/buffer/buffer.c
        src/misc/buffer/buffer.h
        src/misc/types.h
        src/net/http/entity.h
        src/net/http/parser.h
)

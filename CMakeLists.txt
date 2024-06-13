cmake_minimum_required(VERSION 3.28)
project(mars C)

set(CMAKE_C_STANDARD 11)

find_package(unity)

add_executable(mars main.c
        src/net/http/mars_http_parser.c
        src/net/http/mars_http_parser.h
        src/net/http/mars_http.c
        src/net/http/mars_http.h
        src/net/http/mars_http_statuses.c
        src/misc/buffer/buffer.c
        src/misc/buffer/buffer.h
        src/misc/types.h
)
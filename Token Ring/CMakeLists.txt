cmake_minimum_required(VERSION 3.12)
project(pulnar_agnieszka_1)

set(CMAKE_C_STANDARD 11)

add_executable(pulnar_agnieszka_1 client.c tcp_client.c tcp_client.h udp_client.c udp_client.h client.h)

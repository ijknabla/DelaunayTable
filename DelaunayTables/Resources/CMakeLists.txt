cmake_minimum_required(VERSION 3.0)

project(DelaunayTable C)

include(CTest)

find_package(BLAS)

add_subdirectory(C-Sources)

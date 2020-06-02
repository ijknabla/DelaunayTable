
add_library(
    DelaunayTable SHARED
    DelaunayTable.IO.c
    DelaunayTable.Geometry.c
    DelaunayTable.Container.c
)

target_include_directories(
    DelaunayTable
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(
    test
)

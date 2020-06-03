
add_library(
    DelaunayTable SHARED
    DelaunayTable.Geometry.c
    DelaunayTable.Container.c
    DelaunayTable.IndexVector.c
    DelaunayTable.PolygonTree.c
    DelaunayTable.Neighbor.c
    DelaunayTable.IO.c
)

target_include_directories(
    DelaunayTable
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    DelaunayTable
    ${BLAS_LIBRARIES}
)

add_subdirectory(
    test
)

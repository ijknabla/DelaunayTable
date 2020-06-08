
add_library(
    DelaunayTable SHARED
    DelaunayTable.Geometry.c
    DelaunayTable.Container.c
    DelaunayTable.ResourceStack.c
    DelaunayTable.IndexVector.c
    DelaunayTable.PolygonTree.c
    DelaunayTable.Neighbor.c
    DelaunayTable.IO.c
    DelaunayTable.c
)

target_compile_definitions(
    DelaunayTable PUBLIC -DNoModelicaStdLib
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

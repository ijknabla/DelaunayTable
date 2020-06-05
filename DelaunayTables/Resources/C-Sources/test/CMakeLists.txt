
add_executable(
    testVector__append
    Vector__append.c
)
target_link_libraries(
    testVector__append
    DelaunayTable
)

add_test(
    NAME "Vector.append"
    COMMAND $<TARGET_FILE:testVector__append>
)


add_executable(
    testVector__copy
    Vector__copy.c
)
target_link_libraries(
    testVector__copy
    DelaunayTable
)

add_test(
    NAME "Vector.copy"
    COMMAND $<TARGET_FILE:testVector__copy>
)


add_executable(
    testHashMap__set
    HashMap__set.c
)
target_link_libraries(
    testHashMap__set
    DelaunayTable
)

add_test(
    NAME "HashMap.set"
    COMMAND $<TARGET_FILE:testHashMap__set>
)


add_executable(
    testDivisionRatio__2D
    DivisionRatio__2D.c
)
target_link_libraries(
    testDivisionRatio__2D
    DelaunayTable
)

add_test(
    NAME "DivisionRatio.2D"
    COMMAND $<TARGET_FILE:testDivisionRatio__2D>
)


add_executable(
    testDivisionRatio__4D
    DivisionRatio__4D.c
)
target_link_libraries(
    testDivisionRatio__4D
    DelaunayTable
)

add_test(
    NAME "DivisionRatio.4D"
    COMMAND $<TARGET_FILE:testDivisionRatio__4D>
)


add_executable(
    testCircumsphere__2D
    Circumsphere__2D.c
)
target_link_libraries(
    testCircumsphere__2D
    DelaunayTable
)

add_test(
    NAME "Circumsphere.2D"
    COMMAND $<TARGET_FILE:testCircumsphere__2D>
)


add_executable(
    testCircumsphere__4D
    Circumsphere__4D.c
)
target_link_libraries(
    testCircumsphere__4D
    DelaunayTable
)

add_test(
    NAME "Circumsphere.4D"
    COMMAND $<TARGET_FILE:testCircumsphere__4D>
)


add_executable(
    testPolygonTree__Binary
    PolygonTree__Binary.c
)
target_link_libraries(
    testPolygonTree__Binary
    DelaunayTable
)

add_test(
    NAME "PolygonTree.Binary"
    COMMAND $<TARGET_FILE:testPolygonTree__Binary>
)


add_executable(
    testSimple_I2O1x4
    Simple_I2O1x4.c
)
target_link_libraries(
    testSimple_I2O1x4
    DelaunayTable
)

add_test(
    NAME "Simple.I2O1x4"
    COMMAND $<TARGET_FILE:testSimple_I2O1x4>
)


add_executable(
    testSimple_I2O1x5
    Simple_I2O1x5.c
)
target_link_libraries(
    testSimple_I2O1x5
    DelaunayTable
)

add_test(
    NAME "Simple.I2O1x5"
    COMMAND $<TARGET_FILE:testSimple_I2O1x5>
)


add_executable(
    testSimple_I3O1x8
    Simple_I3O1x8.c
)
target_link_libraries(
    testSimple_I3O1x8
    DelaunayTable
)

add_test(
    NAME "Simple.I3O1x8"
    COMMAND $<TARGET_FILE:testSimple_I3O1x8>
)

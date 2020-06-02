
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


add_executable(
    testList__append
    List__append.c
)

target_link_libraries(
    testList__append
    DelaunayTable
)

add_test(
  NAME List__append
  COMMAND $<TARGET_FILE:testList__append>
)

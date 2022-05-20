
add_executable(user1 swmm6-user1.c)
target_link_libraries(user1 PUBLIC swmm6)

add_executable(user2 swmm6-user2.c)
target_link_libraries(user2 PUBLIC swmm6)

add_executable(test-hash hash_test.c)
target_link_libraries(test-hash PUBLIC swmm6)

add_test(NAME test-hash COMMAND test-hash)

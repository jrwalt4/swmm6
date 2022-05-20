
add_library(swmm6 SHARED
  extension.c
  hash.c
  input.c
  node.c
  object.c
  provider.c
  simulation.c
  swmm6.c
)
target_include_directories(swmm6 PUBLIC ${CMAKE_CURRENT_LIST_DIR} ../include)

find_package(SQLite3 REQUIRED)
target_link_libraries(swmm6 PRIVATE SQLite::SQLite3)

target_compile_options(swmm6 PRIVATE -Wall -Wextra)

add_executable(swmm6cli main.c)
set_target_properties(swmm6cli PROPERTIES OUTPUT_NAME main)
target_link_libraries(swmm6cli PUBLIC swmm6)

add_test(NAME test-cli COMMAND swmm6cli ${CMAKE_SOURCE_DIR}/test/swmm6test.db)



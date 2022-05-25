
add_library(swmm6 SHARED
  swmm6.cc
  swmm6_int.hh

  error.cc
  error.hh
  extension.cc
  input.cc
  input.hh
  node.cc
  object.cc
  provider.cc
  provider.hh
  simulation.cc
  simulation.hh
)
target_include_directories(swmm6 PUBLIC ${CMAKE_CURRENT_LIST_DIR} ../include)

find_package(SQLite3 REQUIRED)
target_link_libraries(swmm6 PRIVATE SQLite::SQLite3)

target_compile_options(swmm6 PRIVATE -Wall -Wextra)

target_compile_features(swmm6 PRIVATE cxx_std_17)

add_executable(swmm6cli main.cc)
set_target_properties(swmm6cli PROPERTIES OUTPUT_NAME main)
target_link_libraries(swmm6cli PUBLIC swmm6)

add_test(NAME test-cli COMMAND swmm6cli ${CMAKE_SOURCE_DIR}/test/swmm6test.db)



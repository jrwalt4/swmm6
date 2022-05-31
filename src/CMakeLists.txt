add_library(swmm6iface INTERFACE)
target_include_directories(swmm6iface INTERFACE ${CMAKE_CURRENT_LIST_DIR} ../include)
target_compile_features(swmm6iface INTERFACE cxx_std_17)
target_compile_options(swmm6iface INTERFACE -Wall -Wextra)

add_subdirectory(nodes)

add_library(swmm6 SHARED
  swmm6.cc
  swmm6_int.hh
  swmm6_ext.cc

  error.cc
  error.hh
  input.cc
  input.hh
  node.cc
  node.hh
  object.hh
  provider.cc
  provider.hh
  simulation.cc
  simulation.hh
)

find_package(SQLite3 REQUIRED)
target_link_libraries(swmm6
  PUBLIC swmm6iface
  PRIVATE swmm6nodes
  PRIVATE SQLite::SQLite3
)

add_executable(swmm6cli main.cc)
set_target_properties(swmm6cli PROPERTIES OUTPUT_NAME main)
target_link_libraries(swmm6cli PUBLIC swmm6)

add_test(NAME test-cli COMMAND swmm6cli ${CMAKE_SOURCE_DIR}/test/swmm6test.db)



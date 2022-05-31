
add_library(swmm6node OBJECT
    junction.cc
    junction.hh
)

target_link_libraries(swmm6node PUBLIC swmm6iface)


add_library(swmm6nodes OBJECT
    junction.cc
    junction.hh
)

target_link_libraries(swmm6nodes PUBLIC swmm6iface)

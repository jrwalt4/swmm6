
configure_file(setup.py.in setup.py)

add_custom_command(
    OUTPUT pyswmm6.c
    COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/setup.py build_ext
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

add_custom_target(build-pyswmm6
    COMMAND python3 ${CMAKE_CURRENT_LIST_DIR}/main.py ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/pyswmm6.c
)

add_test(
    pyswmm6-main
    COMMAND python3 ${CMAKE_CURRENT_LIST_DIR}/main.py ${CMAKE_CURRENT_BINARY_DIR}
)

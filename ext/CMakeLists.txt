
set(PYSWMM6_CYTHON_SOURCE ${CMAKE_CURRENT_LIST_DIR}/pyswmm6.pyx)
set(PYSWMM6_CYTHON_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pyswmm6.c)
set(PYSWMM6_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

# generate the cython C source file
add_custom_command(
    OUTPUT ${PYSWMM6_CYTHON_OUTPUT}
    COMMAND cython ${PYSWMM6_CYTHON_SOURCE} -3 -o ${PYSWMM6_CYTHON_OUTPUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# build python extension from generate C source file
# NOTE: setup.py is configured below (need to define target first
# to use NAME property)
add_custom_target(
    pyswmm6
    COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/setup.py build_ext --inplace
    DEPENDS ${PYSWMM6_CYTHON_OUTPUT}
)

# get module name to use in `configure_file` below
get_target_property(PYSWMM6_MODULE_NAME pyswmm6 NAME)

configure_file(setup.py.in setup.py @ONLY)
configure_file(main.py.in main.py @ONLY)

add_test(
    pyswmm6-main
    COMMAND python3 ${CMAKE_CURRENT_BINARY_DIR}/main.py
)

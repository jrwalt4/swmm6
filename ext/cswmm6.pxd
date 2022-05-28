
cdef extern from "swmm6.h":
    ctypedef struct swmm6

    cdef int swmm6_open(const char* input, swmm6** prj)

cdef extern from "swmm6_ext.h":
    ctypedef struct swmm6_input

    ctypedef struct swmm6_input_cursor

    ctypedef struct swmm6_io_module:
        @staticmethod
        cdef xOpen(const char* name, swmm6_input** out)

        @staticmethod
        cdef xOpenCursor(swmm6_input* input, swmm6_input_cursor** out)

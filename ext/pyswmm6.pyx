#from cswmm6 cimport swmm6_io_module, swmm6_input, swmm6_input_cursor

def say_hello_to(name):
    print("Hello %s!" % name)

""""
cdef class SwmmInput:
    def __cinit__(self, const char* name):
        self._c_io_module = swmm6_io_module()
        self._c_io_module.xOpen = self.open_cursor

    @staticmethod
    cdef int open_cursor(const char* name, swmm6_input** out):
        return 0

"""

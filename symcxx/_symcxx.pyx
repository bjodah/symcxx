# -*- coding: utf-8 -*-
# distutils: language = c++
# distutils: extra_compile_args = -std=c++11 -I./include

from symcxx cimport NameSpace as CppNameSpace
from symcxx cimport idx_t

cdef class NameSpace:
    cdef CppNameSpace *thisptr

    def __cinit__(self, idx_t n):
        self.thisptr = new CppNameSpace(n)

    def __dealloc__(self):
        del self.thisptr

    property nsymbs:
        def __get__(self):
            return self.thisptr.nsymbs

    def make_integer(self, intgr):
        return self.thisptr.make_integer(intgr)

    def make_nan(self):
        return self.thisptr.make_nan()

    def evalf(self, idx_t idx, double [::1] inp):
        return self.thisptr.evalf(idx, &inp[0])

    def diff(self, idx0, idx1):
        return self.thisptr.diff(idx0, idx1)

    # Unary:
    def exp(self, idx):
        return self.thisptr.exp(idx)

    # Binary:
    def add2(self, idx0, idx1):
        return self.thisptr.add2(idx0, idx1)

    def mul2(self, idx0, idx1):
        return self.thisptr.mul2(idx0, idx1)

    def div(self, idx0, idx1):
        return self.thisptr.div(idx0, idx1)

    def sub(self, idx0, idx1):
        return self.thisptr.sub(idx0, idx1)

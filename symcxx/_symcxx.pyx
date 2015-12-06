# -*- coding: utf-8 -*-
# distutils: language = c++
# distutils: extra_compile_args = -std=c++11 -I./include


from libcpp cimport bool
ctypedef unsigned int idx_t

cdef extern from "symcxx/core.hpp" namespace "symcxx":
    cdef cppclass NameSpace:
        idx_t nsymbs
        NameSpace(idx_t)
        idx_t make_symbol(idx_t)
        idx_t make_integer(int)
        idx_t make_float(double)
        idx_t make_nan()

        bool evalb(idx_t, const double *)
        double evalf(idx_t, const double *)
        idx_t diff(const idx_t, const idx_t)

        idx_t exp(idx_t)

        idx_t add2(idx_t, idx_t)
        idx_t mul2(idx_t, idx_t)
        idx_t div(idx_t, idx_t)
        idx_t sub(idx_t, idx_t)

        idx_t lt(idx_t, idx_t)
        idx_t le(idx_t, idx_t)
        idx_t eq(idx_t, idx_t)
        idx_t ne(idx_t, idx_t)
        idx_t ge(idx_t, idx_t)
        idx_t gt(idx_t, idx_t)

cdef class NameSpace:
    cdef CppNameSpace *thisptr

    def __cinit__(self, idx_t n):
        self.thisptr = new CppNameSpace(n)

    def __dealloc__(self):
        del self.thisptr

    property nsymbs:
        def __get__(self):
            return self.thisptr.nsymbs

    def make_symbol(self, symb_idx):
        return self.thisptr.make_symbol(symb_idx)

    def make_integer(self, intgr):
        return self.thisptr.make_integer(intgr)

    def make_nan(self):
        return self.thisptr.make_nan()

    def evalb(self, idx_t idx, double [::1] inp):
        return self.thisptr.evalb(idx, &inp[0])

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

    # Relational
    def lt(self, idx0, idx1):
        return self.thisptr.lt(idx0, idx1)

    def le(self, idx0, idx1):
        return self.thisptr.le(idx0, idx1)

    def eq(self, idx0, idx1):
        return self.thisptr.eq(idx0, idx1)

    def ne(self, idx0, idx1):
        return self.thisptr.ne(idx0, idx1)

    def ge(self, idx0, idx1):
        return self.thisptr.ge(idx0, idx1)

    def gt(self, idx0, idx1):
        return self.thisptr.gt(idx0, idx1)

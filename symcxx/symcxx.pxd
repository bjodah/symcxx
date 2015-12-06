from libcpp cimport bool
ctypedef unsigned int idx_t

cdef extern from "symcxx/core.hpp" namespace "symcxx":
    cdef cppclass NameSpace:
        const idx_t nsymbs
        NameSpace(idx_t)
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

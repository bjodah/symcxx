ctypedef unsigned int idx_t

cdef extern from "symcxx/core.hpp" namespace "symcxx":
    cdef cppclass NameSpace:
        const idx_t nsymbs
        NameSpace(idx_t)
        idx_t make_integer(int)
        idx_t make_float(double)
        idx_t make_nan()

        double evalf(idx_t, const double *)
        idx_t diff(const idx_t, const idx_t)

        idx_t exp(idx_t)

        idx_t add2(idx_t, idx_t)
        idx_t mul2(idx_t, idx_t)
        idx_t div(idx_t, idx_t)
        idx_t sub(idx_t, idx_t)

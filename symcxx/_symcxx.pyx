# distutils: language = c++
# distutils: extra_compile_args = -std=c++11 -I./include


# THIS IS A GENERATED FILE DO NOT EDIT

from libcpp cimport bool
ctypedef unsigned int idx_t

import numpy as np

cdef extern from "symcxx/core.hpp" namespace "symcxx":
    cdef cppclass NameSpace:
        idx_t n_symbs
        NameSpace(idx_t)
        idx_t make_symbol(idx_t)
        idx_t make_integer(int)
        idx_t make_float(double)
        idx_t make_nan()

        bool evalb(idx_t, const double *)
        double evalf(idx_t, const double *)
        idx_t diff(const idx_t, const idx_t)

        idx_t cos(idx_t)
        idx_t sin(idx_t)
        idx_t tan(idx_t)
        idx_t acos(idx_t)
        idx_t asin(idx_t)
        idx_t atan(idx_t)
        idx_t cosh(idx_t)
        idx_t sinh(idx_t)
        idx_t tanh(idx_t)
        idx_t acosh(idx_t)
        idx_t asinh(idx_t)
        idx_t atanh(idx_t)
        idx_t exp(idx_t)
        idx_t log(idx_t)
        idx_t log10(idx_t)
        idx_t exp2(idx_t)
        idx_t expm1(idx_t)
        idx_t log1p(idx_t)
        idx_t log2(idx_t)
        idx_t logb(idx_t)
        idx_t sqrt(idx_t)
        idx_t cbrt(idx_t)
        idx_t erf(idx_t)
        idx_t erfc(idx_t)
        idx_t tgamma(idx_t)
        idx_t lgamma(idx_t)

        idx_t sub(idx_t, idx_t)
        idx_t div(idx_t, idx_t)
        idx_t atan2(idx_t, idx_t)
        idx_t pow(idx_t, idx_t)
        idx_t hypot(idx_t, idx_t)
        idx_t add2(idx_t, idx_t)
        idx_t mul2(idx_t, idx_t)
        idx_t lt(idx_t, idx_t)
        idx_t le(idx_t, idx_t)
        idx_t eq(idx_t, idx_t)
        idx_t ne(idx_t, idx_t)
        idx_t ge(idx_t, idx_t)
        idx_t gt(idx_t, idx_t)


class _IndexedObject(object):

    def __init__(self, idx, namespace, **kwargs):
        self.idx = idx
        self.ns = namespace
        self.__dict__.update(kwargs)

    def __eq__(self, other):
        other = _implicit_conversion(self.ns, other)
        if self.idx == other.idx and self.ns == other.ns:
            return True
        else:
            return False

    def __add__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns._Add2(self.idx, other.idx), self.ns)

    def __radd__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns._Add2(other.idx, self.idx), self.ns)

    def __sub__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns._Sub(self.idx, other.idx), self.ns)

    def __rsub__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns._Sub(other.idx, self.idx), self.ns)

    def __mul__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns._Mul2(self.idx, other.idx), self.ns)

    def __rmul__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns._Mul2(other.idx, self.idx), self.ns)

    def __truediv__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns._Div(self.idx, other.idx), self.ns)

    def __rtruediv__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns._Div(other.idx, self.idx), self.ns)

    def evalb(self, inp):
        return self.ns.evalb(self.idx, inp)

    def evalf(self, inp):
        return self.ns.evalf(self.idx, inp)

    def diff(self, wrt):
        return self.ns.diff(self.idx, wrt.idx)


def _implicit_conversion(ns, other):
    if isinstance(other, _IndexedObject):  # or look for 'ns' and 'idx'?
        return other
    elif isinstance(other, int):
        return _IndexedObject(ns.make_integer(other), ns)
    elif isinstance(other, float):
        return _IndexedObject(ns.make_float(other), ns)
    else:
        raise NotImplementedError


cdef class PyNameSpace:
    cdef NameSpace *thisptr
    cdef readonly list symbol_names

    def __cinit__(self, idx_t n):
        self.symbol_names = []
        self.thisptr = new NameSpace(n)

    def __dealloc__(self):
        del self.thisptr

    property n_symbs:
        def __get__(self):
            return self.thisptr.n_symbs

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
    def _Cos(self, idx):
        return self.thisptr.cos(idx)

    def cos(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Cos(obj), self)
    def _Sin(self, idx):
        return self.thisptr.sin(idx)

    def sin(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Sin(obj), self)
    def _Tan(self, idx):
        return self.thisptr.tan(idx)

    def tan(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Tan(obj), self)
    def _Acos(self, idx):
        return self.thisptr.acos(idx)

    def acos(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Acos(obj), self)
    def _Asin(self, idx):
        return self.thisptr.asin(idx)

    def asin(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Asin(obj), self)
    def _Atan(self, idx):
        return self.thisptr.atan(idx)

    def atan(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Atan(obj), self)
    def _Cosh(self, idx):
        return self.thisptr.cosh(idx)

    def cosh(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Cosh(obj), self)
    def _Sinh(self, idx):
        return self.thisptr.sinh(idx)

    def sinh(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Sinh(obj), self)
    def _Tanh(self, idx):
        return self.thisptr.tanh(idx)

    def tanh(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Tanh(obj), self)
    def _Acosh(self, idx):
        return self.thisptr.acosh(idx)

    def acosh(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Acosh(obj), self)
    def _Asinh(self, idx):
        return self.thisptr.asinh(idx)

    def asinh(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Asinh(obj), self)
    def _Atanh(self, idx):
        return self.thisptr.atanh(idx)

    def atanh(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Atanh(obj), self)
    def _Exp(self, idx):
        return self.thisptr.exp(idx)

    def exp(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Exp(obj), self)
    def _Log(self, idx):
        return self.thisptr.log(idx)

    def log(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Log(obj), self)
    def _Log10(self, idx):
        return self.thisptr.log10(idx)

    def log10(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Log10(obj), self)
    def _Exp2(self, idx):
        return self.thisptr.exp2(idx)

    def exp2(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Exp2(obj), self)
    def _Expm1(self, idx):
        return self.thisptr.expm1(idx)

    def expm1(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Expm1(obj), self)
    def _Log1p(self, idx):
        return self.thisptr.log1p(idx)

    def log1p(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Log1p(obj), self)
    def _Log2(self, idx):
        return self.thisptr.log2(idx)

    def log2(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Log2(obj), self)
    def _Logb(self, idx):
        return self.thisptr.logb(idx)

    def logb(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Logb(obj), self)
    def _Sqrt(self, idx):
        return self.thisptr.sqrt(idx)

    def sqrt(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Sqrt(obj), self)
    def _Cbrt(self, idx):
        return self.thisptr.cbrt(idx)

    def cbrt(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Cbrt(obj), self)
    def _Erf(self, idx):
        return self.thisptr.erf(idx)

    def erf(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Erf(obj), self)
    def _Erfc(self, idx):
        return self.thisptr.erfc(idx)

    def erfc(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Erfc(obj), self)
    def _Tgamma(self, idx):
        return self.thisptr.tgamma(idx)

    def tgamma(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Tgamma(obj), self)
    def _Lgamma(self, idx):
        return self.thisptr.lgamma(idx)

    def lgamma(self, obj):
        obj = _implicit_conversion(self, obj)
        return _IndexedObject(self._Lgamma(obj), self)

    # Binary:
    def _Sub(self, idx0, idx1):
        return self.thisptr.sub(idx0, idx1)
    def sub(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Sub(obj0, obj1), self)
    def _Div(self, idx0, idx1):
        return self.thisptr.div(idx0, idx1)
    def div(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Div(obj0, obj1), self)
    def _Atan2(self, idx0, idx1):
        return self.thisptr.atan2(idx0, idx1)
    def atan2(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Atan2(obj0, obj1), self)
    def _Pow(self, idx0, idx1):
        return self.thisptr.pow(idx0, idx1)
    def pow(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Pow(obj0, obj1), self)
    def _Hypot(self, idx0, idx1):
        return self.thisptr.hypot(idx0, idx1)
    def hypot(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Hypot(obj0, obj1), self)
    def _Add2(self, idx0, idx1):
        return self.thisptr.add2(idx0, idx1)
    def add2(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Add2(obj0, obj1), self)
    def _Mul2(self, idx0, idx1):
        return self.thisptr.mul2(idx0, idx1)
    def mul2(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Mul2(obj0, obj1), self)
    def _Lt(self, idx0, idx1):
        return self.thisptr.lt(idx0, idx1)
    def lt(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Lt(obj0, obj1), self)
    def _Le(self, idx0, idx1):
        return self.thisptr.le(idx0, idx1)
    def le(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Le(obj0, obj1), self)
    def _Eq(self, idx0, idx1):
        return self.thisptr.eq(idx0, idx1)
    def eq(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Eq(obj0, obj1), self)
    def _Ne(self, idx0, idx1):
        return self.thisptr.ne(idx0, idx1)
    def ne(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Ne(obj0, obj1), self)
    def _Ge(self, idx0, idx1):
        return self.thisptr.ge(idx0, idx1)
    def ge(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Ge(obj0, obj1), self)
    def _Gt(self, idx0, idx1):
        return self.thisptr.gt(idx0, idx1)
    def gt(self, obj0, obj1):
        obj0 = _implicit_conversion(self, obj0)
        obj1 = _implicit_conversion(self, obj1)
        return _IndexedObject(self._Gt(obj0, obj1), self)

    def Number(self, val):
        if isinstance(val, int):
            return _IndexedObject(self.make_integer(val), self)
        else:
            return _IndexedObject(self.make_float(val), self)

    def Symbol(self, name):
        symb_idx = self.make_symbol(len(self.symbol_names))
        self.symbol_names.append(name)
        return _IndexedObject(symb_idx, self)

    def symarray(self, prefix, shape, real=True):
        if real is not True:
            raise NotImplementedError
        arr = np.empty(shape, dtype=object)
        for index in np.ndindex(shape):
            arr[index] = self.Symbol('%s_%s' % (
                prefix, '_'.join(map(str, index))))
        return arr

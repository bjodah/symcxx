# -*- coding: utf-8 -*-

from __future__ import absolute_import, division, print_function

import functools

from ._symcxx import NameSpace as CyNameSpace


def _implicit_conversion(ns, other):
    if isinstance(other, _IndexedObject):  # or look for 'ns' and 'idx'?
        return other
    elif isinstance(other, int):
        return _IndexedObject(ns.make_integer(other), ns)
    elif isinstance(other, float):
        return _IndexedObject(ns.make_float(other), ns)
    else:
        raise NotImplementedError

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
        return _IndexedObject(self.ns.add2(self.idx, other.idx), self.ns)

    def __radd__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns.add2(other.idx, self.idx), self.ns)

    def __sub__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns.sub(self.idx, other.idx), self.ns)

    def __rsub__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns.sub(other.idx, self.idx), self.ns)

    def __mul__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns.mul2(self.idx, other.idx), self.ns)

    def __rmul__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns.mul2(other.idx, self.idx), self.ns)

    def __truediv__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns.div(self.idx, other.idx), self.ns)

    def __rtruediv__(self, other):
        other = _implicit_conversion(self.ns, other)
        return _IndexedObject(self.ns.div(other.idx, self.idx), self.ns)

    def evalb(self, inp):
        return self.ns.evalb(self.idx, inp)

    def evalf(self, inp):
        return self.ns.evalf(self.idx, inp)

    def diff(self, wrt):
        return self.ns.diff(self.idx, wrt.idx)


class NameSpace(CyNameSpace):

    def __init__(self, n):
        self.symbols = []

    def Number(self, val):
        if isinstance(val, int):
            return self.make_integer(val)
        else:
            return self.make_float(val)

    def Symbol(self, name):
        print(name)
        n_named = len(self.symbols)
        if n_named < self.nsymbs:
            symb = _IndexedObject(n_named, self, name=name)
            self.symbols.append(symb)
            return symb
        else:
            raise ValueError("NameSpace out of space.")

    def symarray(self, prefix, shape, real=True):
        if real is not True:
            raise NotImplementedError
        arr = np.empty(shape, dtype=object)
        for index in np.ndindex(shape):
            arr[index] = self.Symbol('%s_%s' % (
                prefix, '_'.join(map(str, index))))
        return arr

    # Unary:
    def exp(self, x):
        x = _implicit_conversion(self, x)
        return _IndexedObject(self.exp(x), self)

    # Binary:

    # Relational

    def Lt(self, a, b):
        a = _implicit_conversion(self, a)
        b = _implicit_conversion(self, b)
        return _IndexedObject(self.lt(a.idx, b.idx), self)

    def Le(self, a, b):
        a = _implicit_conversion(self, a)
        b = _implicit_conversion(self, b)
        return _IndexedObject(self.le(a.idx, b.idx), self)

    def Eq(self, a, b):
        a = _implicit_conversion(self, a)
        b = _implicit_conversion(self, b)
        return _IndexedObject(self.eq(a.idx, b.idx), self)

    def Ne(self, a, b):
        a = _implicit_conversion(self, a)
        b = _implicit_conversion(self, b)
        return _IndexedObject(self.ne(a.idx, b.idx), self)

    def Ge(self, a, b):
        a = _implicit_conversion(self, a)
        b = _implicit_conversion(self, b)
        return _IndexedObject(self.ge(a.idx, b.idx), self)

    def Gt(self, a, b):
        a = _implicit_conversion(self, a)
        b = _implicit_conversion(self, b)
        return _IndexedObject(self.gt(a.idx, b.idx), self)

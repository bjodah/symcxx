# -*- coding: utf-8 -*-

from __future__ import absolute_import, division, print_function

import functools

from ._symcxx import NameSpace as CyNameSpace


class _IndexedObject(object):

    def __init__(self, idx, namespace, **kwargs):
        self.idx = idx
        self.ns = namespace
        self.__dict__.update(kwargs)

    def _implicit_conversion(self, other):
        if isinstance(other, _IndexedObject):  # or look for 'ns' and 'idx'?
            return other
        elif isinstance(other, int):
            return _IndexedObject(self.ns.make_integer(other), self.ns)
        elif isinstance(other, float):
            return _IndexedObject(self.ns.make_float(other), self.ns)
        else:
            raise NotImplementedError

    def __add__(self, other):
        other = self._implicit_conversion(other)
        return _IndexedObject(self.ns.add2(self.idx, other.idx), self.ns)

    def __mul__(self, other):
        other = self._implicit_conversion(other)
        return _IndexedObject(self.ns.mul2(self.idx, other.idx), self.ns)

    def __truediv__(self, other):
        other = self._implicit_conversion(other)
        return _IndexedObject(self.ns.div(self.idx, other.idx), self.ns)

    def evalf(self, inp):
        return self.ns.evalf(self.idx, inp)

    def diff(self, wrt):
        return self.ns.diff(self.idx, wrt.idx)

    


class NameSpace(CyNameSpace):

    def __init__(self, n):
        self.symbols = []

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
            

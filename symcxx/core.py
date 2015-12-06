from ._symcxx import NameSpace as CyNameSpace

class _Expression(object):

    def __init__(self, idx, namespace):
        self.idx = idx
        self.ns = namespace

    def evalf(self, inp):
        return self.ns.evalf(self.idx, inp)

    def diff(self, wrt):
        return self.ns.diff(self.idx, wrt.idx)


class _Symbol(object):

    def __init__(self, idx, name, namespace):
        self.idx = idx
        self.name = name
        self.ns = namespace

    def __add__(self, other):
        return _Expression(self.ns.add2(self.idx, other.idx), self.ns)

    def __mul__(self, other):
        return _Expression(self.ns.mul2(self.idx, other.idx), self.ns)

    def __truediv__(self, other):
        return _Expression(self.ns.div(self.idx, other.idx), self.ns)


class NameSpace(CyNameSpace):
    symbols = []

    def Symbol(self, name):
        n_named = len(self.symbols)
        if n_named < self.nsymbs:
            symb = _Symbol(n_named, name, self)
            self.symbols.append(symb)
            return symb
        else:
            raise ValueError("NameSpace out of space.")

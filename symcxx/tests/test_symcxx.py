# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

import numpy as np
from symcxx import NameSpace

def test_NameSpace_Symbol():
    ns = NameSpace(2)
    x = ns.Symbol('x')
    y = ns.Symbol('y')
    add = x + y
    result = add.evalf(np.array([3., 5.]))
    assert abs(result - 8) < 1e-15

def test_division():
    ns = NameSpace(2)
    assert ns.symbols == []

    x, y = map(ns.Symbol, 'x y'.split())
    zero_over_x_1 = 0/x
    zero_over_x_2 = 0/x
    assert zero_over_x_1 == zero_over_x_2

    expr = x/y
    assert abs(expr.evalf(np.array([3., 7.])) - 3/7) < 1e-15

    assert x/3 == x/3
    assert 3/y == 3/y
    assert 3/x != 3/y

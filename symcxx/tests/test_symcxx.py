# -*- coding: utf-8 -*-
from __future__ import absolute_import, division, print_function

import math

import numpy as np
import pytest

from symcxx import NameSpace


def test_NameSpace_Symbol():
    ns = NameSpace(2)
    x = ns.Symbol('x')
    y = ns.Symbol('y')
    add = x + y
    result = add.evalf(np.array([3., 5.]))
    assert abs(result - 8) < 1e-15

    sub = x - y
    result = sub.evalf(np.array([3., 5.]))
    assert abs(result + 2) < 1e-15


def test_NameSpace_relational():
    ns = NameSpace(2)
    x = ns.Symbol('x')
    y = ns.Symbol('y')
    lt = ns.lt(x, y)
    le = ns.le(x, y)
    eq = ns.eq(x, y)
    ne = ns.ne(x, y)
    ge = ns.ge(x, y)
    gt = ns.gt(x, y)
    assert lt.evalb(np.array([3., 5.])) is True
    assert lt.evalb(np.array([5., 5.])) is False
    assert le.evalb(np.array([3., 5.])) is True
    assert le.evalb(np.array([5., 5.])) is True
    assert eq.evalb(np.array([3., 5.])) is False
    assert eq.evalb(np.array([5., 5.])) is True
    assert ne.evalb(np.array([3., 5.])) is True
    assert ne.evalb(np.array([5., 5.])) is False
    assert ge.evalb(np.array([3., 5.])) is False
    assert ge.evalb(np.array([5., 5.])) is True
    assert gt.evalb(np.array([3., 5.])) is False
    assert gt.evalb(np.array([5., 5.])) is False


@pytest.mark.xfail(reason='to be implemented (not used yet)')
def test_NameSpace_relational_equal():
    ns = NameSpace(2)
    x = ns.Symbol('x')
    y = ns.Symbol('y')
    lt = ns.lt(x, y)
    le = ns.le(x, y)
    eq = ns.eq(x, y)
    ne = ns.ne(x, y)
    ge = ns.ge(x, y)
    gt = ns.gt(x, y)
    assert (x < y) == lt
    assert (x < y) != le
    assert (x <= y) == le
    assert (x <= y) != eq
    assert (x == y) == eq
    assert (x == y) != ne
    assert (x != y) == ne
    assert (x != y) != ge
    assert (x >= y) == ge
    assert (x >= y) != gt
    assert (x > y) == gt
    assert (x > y) != lt


def test_division():
    ns = NameSpace(2)
    assert ns.symbol_names == []

    x, y = map(ns.Symbol, 'x y'.split())
    zero_over_x_1 = 0/x
    zero_over_x_2 = 0/x
    assert zero_over_x_1 == zero_over_x_2

    expr = x/y
    assert abs(expr.evalf(np.array([3., 7.])) - 3/7) < 1e-15

    assert x/3 == x/3
    assert 3/y == 3/y
    assert 3/x != 3/y


def test_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = (3*x)
    deriv = expr.diff(x)
    ref = ns.Number(3)
    assert (deriv == ref) is True
    assert (-x).diff(x) == ns.Number(-1)


@pytest.mark.xfail(reason='to be implemented (not used yet)')
def test_compare_numbers():
    ns = NameSpace()
    three, four = ns.Number(3), ns.Number(4)
    lt = three < four
    assert lt.evalb() is True


def test_str():
    ns = NameSpace(0)
    x = ns.Symbol('x')
    assert str(3*x) in ('Mul2(Integer(3), Symbol(x))',
                        'Mul2(Symbol(x), Integer(3))')


def test_autosimplification():
    ns = NameSpace(0)
    x = ns.Symbol('x')
    assert x - x == 0
    assert x**2 - x*x == 0
    # assert x**2*x**3 - x*x**4 == 0
    # assert x*x*x*x - x**4 == 0
    # assert x*4 - 2*(x*2) == 0
    # assert x*4 - x - x - x - x == 0


def test_trigfuncs_evalf():
    ns = NameSpace()
    cos_pi = ns.cos(ns.pi)
    sin_pi = ns.sin(ns.pi)
    tan_pi = ns.tan(ns.pi)
    assert abs(cos_pi.evalf(np.array([])) + 1) < 1e-15
    assert abs(sin_pi.evalf(np.array([]))) < 1e-15
    assert abs(tan_pi.evalf(np.array([]))) < 1e-15


def test_Div_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = (3*x + 1) / (x - 2)
    deriv = expr.diff(x)
    ref = (3*(3.14-2) - (3*3.14 + 1)) / ((3.14 - 2)**2)
    assert abs(deriv.evalf(np.array([3.14])) - ref) < 1e-15


def test_Pow_diff0():
    ns = NameSpace(7)
    x = ns.Symbol('x')
    expr = (3*x + 1) ** (x - 2)
    deriv = expr.diff(x)
    assert abs(deriv.evalf(np.array([3.14])) - 38.6541588883393) < 1e-13


def test_sin_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.sin(3*x + 1)
    ref = 3*math.cos(3*3.14 + 1)
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_cos_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.cos(3*x + 1)
    ref = -3*math.sin(3*3.14 + 1)
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_exp_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.exp(3*x + 1)
    ref = 3*math.exp(3*3.14 + 1)
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_tan_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.tan(3*x + 1)
    ref = 3*(1 + math.tan(3*3.14 + 1)**2)
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_acos_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.acos(.1*x + .2)
    ref = -0.11657862476093600653
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_asin_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.asin(.1*x + .2)
    ref = 0.11657862476093600653
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_atan_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.atan(.1*x + .2)
    ref = .1/((.1*3.14+.2)**2 + 1)
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_cosh_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.cosh(.1*x + .2)
    ref = .1*math.sinh(.1*3.14 + .2)
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_sinh_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.sinh(.1*x + .2)
    ref = .1*math.cosh(.1*3.14 + .2)
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_tanh_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.tanh(.1*x + .2)
    ref = .1*(1 - math.tanh(.1*3.14 + .2)**2)
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_acosh_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.acosh(3*x + 4)
    ref = 0.224170172808993
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_asinh_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.asinh(3*x + 4)
    ref = 0.222928886183630
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_atanh_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.atanh(3*x + 4)
    ref = -0.0167507554590712
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_log_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.log(3*x + 1)
    ref = 3/(3*3.14 + 1)
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_log10_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.log10(3*x + 1)
    ref = 3/(3*3.14 + 1)/math.log(10)
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_exp2_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.exp2(3*x + 1)
    ref = 3*2**(3*3.14 + 1)*math.log(2)
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-12


def test_expm1():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.expm1(3*x + 1e-16)
    assert abs(expr.evalf(np.array([3.14e-17])) - 1.942e-16) < 1e-18


def test_expm1_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.expm1(3*x + 1e-16)
    ref = 3 + 2.86e-14
    assert abs(expr.diff(x).evalf(np.array([3.14e-15])) - ref) < 1e-15


def test_sqrt_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.sqrt(3*x + 1)
    ref = .5*3/(3*3.14 + 1)**.5
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_cbrt_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.cbrt(3*x + 1)
    ref = 1./(3*3.14 + 1)**(2/3.)
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_erf_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.erf(.1*x - 1)
    ref = .1*2*math.exp(-(.1*3.14-1)**2)/math.pi**.5
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_erfc_diff0():
    ns = NameSpace(1)
    x = ns.Symbol('x')
    expr = ns.erfc(.1*x - 1)
    ref = -.1*2*math.exp(-(.1*3.14-1)**2)/math.pi**.5
    assert abs(expr.diff(x).evalf(np.array([3.14])) - ref) < 1e-15


def test_Matrix():
    ns = NameSpace(2)
    x = ns.Symbol('x')
    y = ns.Symbol('y')
    matrix = ns.Matrix(2, 2, [[x+1, y-2],
                              [x-y, x*y]])
    out = matrix.eval_float(np.array([2., 3.]))
    assert np.allclose(out, [[3, 1],
                             [-1, 6]])


def test_jacobian():
    ns = NameSpace(2)
    x = ns.Symbol('x')
    y = ns.Symbol('y')
    my = ns.Matrix(2, 1, [x+y, x*y])
    mx = ns.Matrix(1, 2, [x, y])
    assert my.shape == (2, 1)
    assert mx.shape == (1, 2)
    J = my.jacobian(mx)
    out = J.eval_float(np.array([2., 3.]))
    assert np.allclose(out, [[1, 1],
                             [3, 2]])


def test_Lambdify():
    ns = NameSpace(2)
    x = ns.Symbol('x')
    y = ns.Symbol('y')
    z = ns.Symbol('z')
    u = ns.Symbol('u')
    v = ns.Symbol('v')
    w = ns.Symbol('w')

    lmb0 = ns.Lambdify((x, y, z, u, v, w), [x, y, z, u, v, w])
    out0 = lmb0(np.array([2., 3, 4, 5, 6, 7]))
    ref0 = [2., 3, 4, 5, 6, 7]
    assert np.allclose(out0.flatten(), ref0)

    lmb3 = ns.Lambdify((z, y, x), [x, y, z])
    out3 = lmb3(np.array([2., 3, 4]))
    ref3 = [4, 3, 2]
    assert np.allclose(out3.flatten(), ref3)

    lmb1 = ns.Lambdify((x, y), [x+y, x-y, x*y, 13*x + 2*y])
    out1 = lmb1(np.array([2.0, 3.0]))
    ref1 = [5, -1, 6, 13*2 + 2*3]
    assert np.allclose(out1.flatten(), ref1)

    lmb2 = ns.Lambdify((y, x), [x+y, x-y, x*y])
    out2 = lmb2(np.array([2.0, 3.0]))
    ref2 = [5, 1, 6]
    assert np.allclose(out2.flatten(), ref2)


def test_equality():
    ns = NameSpace()
    p0 = ns.Symbol('p')
    p1 = ns.Symbol('p')
    assert p0 == p1
    assert p1 == p1


def test_factor():
    ns = NameSpace()
    fortytwo = ns.Number(42)
    factors = ns.factor(fortytwo)
    ref = ns.mul(2, 3, 7)
    assert ref == factors


def test_Dummy():
    ns = NameSpace()
    d = ns.Dummy()
    lmb = ns.Lambdify([d], [3*d, 8])
    assert np.allclose(lmb(np.array([7.0])), [21, 8])


def test_has():
    ns = NameSpace(2)
    x = ns.Symbol('x')
    y = ns.Symbol('y')
    add = x + y
    assert add.has(x)
    assert add.has(y)
    mul = 2*y
    assert mul.has(y)
    assert not mul.has(x)
    expr = (1 + 3/(1 + x))
    assert expr.has(x)
    assert not expr.has(y)

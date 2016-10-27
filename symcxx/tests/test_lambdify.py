# -*- coding: utf-8 -*-

from __future__ import (absolute_import, division, print_function)

import array
import math
import itertools
import pytest

from symcxx import NameSpace

# Tests I wrote for Lambdify in symengine reference 'se'
se = NameSpace()

s0, s1, s2 = map(se.Symbol, 's0,s1,s2'.split(','))


def test_single_arg():
    f = se.lambdify(s0, 2*s0)
    assert f(1) == 2


def test_list_args():
    f = se.lambdify([s0, s1], s0 + s1)
    assert f(1, 2) == 3


def test_sin():
    f = se.lambdify(s0, se.sin(s0))
    assert f(0) == 0.


def test_exponentiation():
    f = se.lambdify(s0, s0**2)
    assert f(-1) == 1
    assert f(0) == 0
    assert f(1) == 1
    assert f(-2) == 4
    assert f(2) == 4
    assert f(2.5) == 6.25


def test_sqrt():
    f = se.lambdify(s0, se.sqrt(s0))
    assert f(0) == 0.
    assert f(1) == 1.
    assert f(4) == 2.
    assert abs(f(2) - 1.414) < 0.001
    assert f(6.25) == 2.5


def test_trig():
    f = se.lambdify([s0], [se.cos(s0), se.sin(s0)])
    d = f(math.pi)
    prec = 1e-11
    assert -prec < d[0] + 1
    assert d[0] + 1 < prec
    assert -prec < d[1]
    assert d[1] < prec
    d = f(3.14159)
    prec = 1e-5
    assert -prec < d[0] + 1
    assert d[0] < prec
    assert -prec < d[1]
    assert d[1] < prec


def allclose(iter_a, iter_b, rtol=1e-10, atol=1e-10):
    for a, b in zip(iter_a, iter_b):
        if not (abs(a-b) < abs(a)*rtol + atol):
            return False
    return True


def test_vector_simple():
    f = se.lambdify((s0, s1, s2), (s2, s1, s0))
    assert allclose(f(3, 2, 1), (1, 2, 3))
    assert allclose(f(1., 2., 3.), (3., 2., 1.))
    with pytest.raises(TypeError):
        f(0)


def test_trig_symbolic():
    f = se.lambdify([s0], [se.cos(s0), se.sin(s0)])
    d = f(math.pi)
    assert abs(d[0] + 1) < 0.0001
    assert abs(d[1] - 0) < 0.0001


def test_trig_float():
    f = se.lambdify([s0], [se.cos(s0), se.sin(s0)])
    d = f(3.14159)
    assert abs(d[0] + 1) < 0.0001
    assert abs(d[1] - 0) < 0.0001


def test_Lambdift_simple():
    h = se.lambdify([s2], [s2])
    assert h(8) == 8
    x = se.Symbol('x')
    i = se.lambdify([x], [x])
    assert i(16) == 16


def test_Lambdify_twice():
    n = 7

    args1 = x, y = se.symbols('x y')

    exprs1 = [x+y, x*y]

    l1 = se.Lambdify(args1, exprs1)

    v1 = l1(range(n, n+len(args1)))
    ref1 = [2*n+1, n*(n+1)]
    assert allclose(v1, ref1)

    args2 = u, v = se.symbols('u v')

    exprs2 = [u - v, u/v]

    mat = se.Matrix(len(exprs2), 1, exprs2)

    mat2 = se.rebuild_debug(args2, mat)
    assert mat2.ns.n_symbs == 2

    l2 = se.Lambdify(args2, exprs2)
    v2 = l2(range(n, n+len(args2)))
    ref2 = [-1, n/(n+1)]
    assert allclose(v2, ref2)


def test_Lambdify():
    n = 7
    args = x, y, z = se.symbols('x y z')
    l = se.Lambdify(args, [x+y+z, x**2, (x-y)/z, x*y*z])
    assert allclose(l(range(n, n+len(args))),
                    [3*n+3, n**2, -1/(n+2), n*(n+1)*(n+2)])


def test_Lambdify_out():
    import numpy as np
    n = 7
    args = x, y, z = se.symbols('x y z')
    exprs = [x+y+z, x**2, (x-y)/z, x*y*z]
    lmb = se.Lambdify(args, exprs)
    inp = np.arange(n, n+len(args))
    out = np.empty(len(exprs))
    lmb(inp, out)
    assert allclose(out,
                    [3*n+3, n**2, -1/(n+2), n*(n+1)*(n+2)])


def _get_2_to_2by2_numpy():
    import numpy as np
    args = x, y = se.symbols('x y')
    exprs = np.array([[x+y+1.0, x*y],
                      [x/y, x**y]])
    l = se.Lambdify(args, exprs)

    def check(A, inp):
        X, Y = inp
        assert abs(A[0, 0] - (X+Y+1.0)) < 1e-15
        assert abs(A[0, 1] - (X*Y)) < 1e-15
        assert abs(A[1, 0] - (X/Y)) < 1e-15
        assert abs(A[1, 1] - (X**Y)) < 1e-13
    return l, check


def test_Lambdify_2dim_numpy():
    import numpy as np
    lmb, check = _get_2_to_2by2_numpy()
    for inp in [(5, 7), np.array([5, 7]), [5.0, 7.0]]:
        A = lmb(inp)
        assert A.shape == (2, 2)
        check(A, inp)


def _get_array():
    X, Y, Z = inp = array.array('d', [1, 2, 3])
    args = x, y, z = se.symbols('x y z')
    exprs = [x+y+z, se.sin(x)*se.log(y)*se.exp(z)]
    ref = [X+Y+Z, math.sin(X)*math.log(Y)*math.exp(Z)]

    def check(arr):
        assert all([abs(x1-x2) < 1e-13 for x1, x2 in zip(ref, arr)])
    return args, exprs, inp, check


def test_array():
    args, exprs, inp, check = _get_array()
    lmb = se.Lambdify(args, exprs)
    out = lmb(inp)
    check(out)


def _get_1_to_2by3_matrix():
    x = se.symbols('x')
    args = x,
    exprs = se.Matrix(2, 3, [x+1, x+2, x+3,
                             1/x, 1/(x*x), 1/(x**3.0)])
    l = se.Lambdify(args, exprs)

    def check(A, inp):
        X, = inp
        assert abs(A[0, 0] - (X+1)) < 1e-15
        assert abs(A[0, 1] - (X+2)) < 1e-15
        assert abs(A[0, 2] - (X+3)) < 1e-15
        assert abs(A[1, 0] - (1/X)) < 1e-15
        assert abs(A[1, 1] - (1/(X*X))) < 1e-15
        assert abs(A[1, 2] - (1/(X**3.0))) < 1e-15
    return l, check


def test_x_times_x():
    l = se.Lambdify([s0], [s0*s0])
    res = l([7.0])
    assert abs(res - 49.) < 1e-15


def test_2dim_Matrix():
    l, check = _get_1_to_2by3_matrix()
    inp = [7]
    check(l(inp), inp)


def test_lambdify_jacobian_simple():
    import numpy as np
    args = se.Matrix(1, 1, [s0])
    v = se.Matrix(1, 1, [s0*s0])
    jac = v.jacobian(args)
    return
    lmb = se.Lambdify(args, jac)
    out = np.empty((1, 1))
    inp = X, = 7,
    lmb(inp, out)
    assert np.atleast_1d(out).shape == (1, 1)
    assert np.allclose(out, [[14.0]])


def test_lambdify_jacobian0():
    import numpy as np
    args = se.Matrix(2, 1, [s0, s1])
    v = se.Matrix(2, 1, [s0**3 * s1, (s0+1)*(s1+1)])
    jac = v.jacobian(args)
    lmb = se.Lambdify([s0, s1], jac)
    out = np.empty((2, 2))
    inp = X, Y = 7, 11
    lmb(inp, out)
    assert out.shape == (2, 2)
    assert np.allclose(out, [[3 * X**2 * Y, X**3],
                             [Y + 1, X + 1]])


def test_lambdify_jacobian1():
    import numpy as np
    x, y = se.symbols('x, y')
    args = se.Matrix(2, 1, [x, y])
    v = se.Matrix(2, 1, [x**3 * y, (x+1)*(y+1)])
    jac = v.jacobian(args)
    lmb = se.Lambdify(args, jac)
    out = np.empty((2, 2))
    inp = X, Y = 7, 11
    lmb(inp, out)
    assert out.shape == (2, 2)
    assert np.allclose(out, [[3 * X**2 * Y, X**3],
                             [Y + 1, X + 1]])


def test_lambdify_jacobian2():
    import numpy as np
    x, y = se.symbols('x, y')
    args = se.Matrix(2, 1, [x, y])
    v = se.Matrix(2, 1, [x**3 * y, (x+1)*(y+1)])
    jac = v.jacobian(args)
    lmb = se.Lambdify(args, jac)
    inp = X, Y = 7, 11
    out = lmb(inp)
    assert np.atleast_1d(out).shape == (2, 2)
    assert np.allclose(out, [[3 * X**2 * Y, X**3],
                             [Y + 1, X + 1]])


def test_itertools_chain():
    args, exprs, inp, check = _get_array()
    l = se.Lambdify(args, exprs)
    inp = itertools.chain([inp[0]], (inp[1],), [inp[2]])
    A = l(tuple(inp))  # numpy 1.10 array failing for itertools chain
    check(A)


def test_3args():
    import numpy as np
    lmb = se.Lambdify([s0, s1, s2], [s0 + (s0-s1)**s2/2 - 1, s1 + (s1 - s0)**s2/2])
    inp = X, Y, Z = 7., 11., 13.
    out = lmb(inp)
    assert out.shape == (2,)
    assert np.allclose(out, [X + (X-Y)**Z/2 - 1, Y + (Y - X)**Z/2])


def test_broadcast():
    import numpy as np
    a = np.linspace(-np.pi, np.pi)
    inp = np.vstack((np.cos(a), np.sin(a))).T  # 50 rows 2 cols
    x, y = se.symbols('x y')
    distance = se.Lambdify([x, y], [se.sqrt(x**2 + y**2)])
    assert np.allclose(distance([inp[0, 0], inp[0, 1]]), [1])
    dists = distance(inp)
    assert dists.shape == (50, 1)
    assert np.allclose(dists, 1)


def test_broadcast_multiple_extra_dimensions():
    import numpy as np
    inp = np.arange(12.).reshape((4, 3, 1))
    x = se.symbols('x')
    cb = se.Lambdify([x], [x**2, x**3])
    assert np.allclose(cb([inp[0, 2]]), [4, 8])
    out = cb(inp)
    assert out.shape == (4, 3, 2)
    assert abs(out[2, 1, 0] - 7**2) < 1e-14
    assert abs(out[2, 1, 1] - 7**3) < 1e-14
    assert abs(out[-1, -1, 0] - 11**2) < 1e-14
    assert abs(out[-1, -1, 1] - 11**3) < 1e-14


def test_2dim_Matrix_broadcast_multiple_extra_dim():
    import numpy as np
    l, check = _get_1_to_2by3_matrix()
    inp = np.arange(1, 4*5*6+1).reshape((4, 5, 6))
    out = l(inp)
    assert out.shape == (4, 5, 6, 2, 3)
    for i, j, k in itertools.product(range(4), range(5), range(6)):
        check(out[i, j, k, ...], (inp[i, j, k],))


@pytest.mark.xfail(reason='not yet implemented')
def test_Lambdify_relational():
    import numpy as np
    x, y = se.symbols('x y')
    cb = se.Lambdify([x, y], [x >= 0, y >= 0])
    assert np.allclose(cb([0.0, 3.0]), [1.0, 1.0])
    inp_arr = np.array([[-0.1, -1.0], [4.0, -5.0]])
    flat_res = np.asarray(cb(inp_arr)).flat
    assert np.allclose(flat_res, [0.0, 0.0, 1.0, 0.0])

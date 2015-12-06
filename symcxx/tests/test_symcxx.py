import numpy as np
from symcxx import NameSpace

# def test_NameSpace():
#     ns = NameSpace(2)
#     expr_id = ns.add2(0, 1)
#     result = ns.evalf(expr_id, np.array([3., 5.]))
#     assert abs(result - 8) < 1e-15

def test_NameSpace_Symbol():
    ns = NameSpace(2)
    x = ns.Symbol('x')
    y = ns.Symbol('y')
    add = x + y
    result = add.evalf(np.array([3., 5.]))
    assert abs(result - 8) < 1e-15

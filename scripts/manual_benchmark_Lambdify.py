from time import clock
import numpy as np
import sympy
import symcxx as _symcxx

symcxx = _symcxx.NameSpace(28)

# Real-life example (ion speciation problem in water chemistry)


def setup(mod):
    tim = clock()
    x = mod.symarray('x', 14)
    p = mod.symarray('p', 14)
    args = np.concatenate((x, p))
    exp = mod.exp
    exprs = [
        x[0] + x[1] - x[4] + 36.252574322669,
        x[0] - x[2] + x[3] + 21.3219379611249,
        x[3] + x[5] - x[6] + 9.9011158998744,
        2*x[3] + x[5] - x[7] + 18.190422234653,
        3*x[3] + x[5] - x[8] + 24.8679190043357,
        4*x[3] + x[5] - x[9] + 29.9336062089226,
        -x[10] + 5*x[3] + x[5] + 28.5520551531262,
        2*x[0] + x[11] - 2*x[4] - 2*x[5] + 32.4401680272417,
        3*x[1] - x[12] + x[5] + 34.9992934135095,
        4*x[1] - x[13] + x[5] + 37.0716199972041,
        (p[0] - p[1] + 2*p[10] + 2*p[11] - p[12] - 2*p[13] + p[2] +
         2*p[5] + 2*p[6] + 2*p[7] + 2*p[8] + 2*p[9] - exp(x[0]) +
         exp(x[1]) - 2*exp(x[10]) - 2*exp(x[11]) + exp(x[12]) +
         2*exp(x[13]) - exp(x[2]) - 2*exp(x[5]) - 2*exp(x[6]) -
         2*exp(x[7]) - 2*exp(x[8]) - 2*exp(x[9])),
        (-p[0] - p[1] - 15*p[10] - 2*p[11] - 3*p[12] - 4*p[13] - 4*p[2] -
         3*p[3] - 2*p[4] - 3*p[6] - 6*p[7] - 9*p[8] - 12*p[9] +
         exp(x[0]) + exp(x[1]) + 15*exp(x[10]) + 2*exp(x[11]) +
         3*exp(x[12]) + 4*exp(x[13]) + 4*exp(x[2]) + 3*exp(x[3]) +
         2*exp(x[4]) + 3*exp(x[6]) + 6*exp(x[7]) + 9*exp(x[8]) +
         12*exp(x[9])),
        (-5*p[10] - p[2] - p[3] - p[6] - 2*p[7] - 3*p[8] - 4*p[9] +
         5*exp(x[10]) + exp(x[2]) + exp(x[3]) + exp(x[6]) + 2*exp(x[7]) +
         3*exp(x[8]) + 4*exp(x[9])),
        (-p[1] - 2*p[11] - 3*p[12] - 4*p[13] - p[4] + exp(x[1]) +
         2*exp(x[11]) + 3*exp(x[12]) + 4*exp(x[13]) + exp(x[4])),
        (-p[10] - 2*p[11] - p[12] - p[13] - p[5] - p[6] - p[7] - p[8] -
         p[9] + exp(x[10]) + 2*exp(x[11]) + exp(x[12]) + exp(x[13]) +
         exp(x[5]) + exp(x[6]) + exp(x[7]) + exp(x[8]) + exp(x[9]))
    ]
    tim = clock() - tim
    try:
        return tim, mod.Lambdify(args, exprs)
    except AttributeError:
        return tim, mod.lambdify(args, exprs)


def evaluate(lmb, niter=5000, unpack=False):
    inp = np.ones(28)
    tim = clock()
    for i in range(niter):
        if unpack:
            res = lmb(*inp)
        else:
            res = lmb(inp)
    tim = clock() - tim
    return tim, res

time_setup, time_evaluate = {}, {}
result_setup, result_evaluate = {}, {}

time_setup['sympy'], result_setup['sympy'] = setup(sympy)
time_setup['symcxx'], result_setup['symcxx'] = setup(symcxx)

time_evaluate['sympy'], result_evaluate['sympy'] = evaluate(
    result_setup['sympy'], unpack=True)
time_evaluate['symcxx'], result_evaluate['symcxx'] = evaluate(
    result_setup['symcxx'])

print('time_setup', time_setup)
print('time_evaluate', time_evaluate)
print('setup speedup: %.1f' % (time_setup['sympy']/time_setup['symcxx']))
print('evaluate speedup: %.1f' % (
    time_evaluate['sympy']/time_evaluate['symcxx']))

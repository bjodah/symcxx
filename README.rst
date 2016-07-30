SymCXX
======

.. image:: http://hera.physchem.kth.se:9090/api/badges/bjodah/symcxx/status.svg
   :target: http://hera.physchem.kth.se:9090/bjodah/symcxx
   :alt: Build status
.. image:: https://img.shields.io/pypi/v/symcxx.svg
   :target: https://pypi.python.org/pypi/symcxx
   :alt: PyPI version
.. image:: https://img.shields.io/pypi/l/symcxx.svg
   :target: https://github.com/bjodah/symcxx/blob/master/LICENSE
   :alt: License
.. image:: http://img.shields.io/badge/benchmarked%20by-asv-green.svg?style=flat
   :target: http://hera.physchem.kth.se/~symcxx/benchmarks
   :alt: airspeedvelocity
.. image:: http://hera.physchem.kth.se/~symcxx/branches/master/htmlcov/coverage.svg
   :target: http://hera.physchem.kth.se/~symcxx/branches/master/htmlcov
   :alt: coverage

`SymCXX <https://github.com/bjodah/symcxx>`_ is an *experimental*
Python package (with its implemented in C++) for symbolic manipulation
with limited scope (as in minimal).
It tries to stay compatible with the API of `SymPy <http://www.sympy.org>`_.
Its main goal is to explore designs related to automatic differentiation and
numerical evaluation similar to SymEngine's Lambdify functionality.

The capabilities include:

- Differentiation (including taking the jacobian of a matrix)
- Numerical evaluation

The above capabilities are exactly what is needed by
`pyodesys <https://pypi.python.org/pypi/pyodesys>`_
and `pyneqsys <https://pypi.python.org/pypi/pyneqsys>`_.

Note that **integers in symcxx may overflow**, i.e. if correctness is important 
please use SymPy/SymEngine.

Documentation
-------------
Currently users may refer to SymPy's / SymEngine's documentation.


Installation
------------
Simplest way to install SymCXX and its (optional) dependencies is to use
the `conda package manager <http://conda.pydata.org/docs/>`_:

::

   $ conda install -c bjodah symcxx pytest
   $ python -m pytest --pyargs symcxx

or pip (requires a C++11 compliant compiler):

::

   $ pip install --user symcxx pytest
   $ python -m pytest --pyargs symcxx


There are no requirements outside the Python standard library.

Source distribution is available here:
`<https://pypi.python.org/pypi/symcxx>`_

Example
-------
Differentiation

.. code:: python

   >>> from symcxx import NameSpace
   >>> ns = NameSpace()
   >>> x, y = map(ns.Symbol, 'x y'.split())
   >>> expr = x*y**2 - ns.tan(2*x)
   >>> print(expr.diff(x))
   Sub(Pow(Symbol(y), Integer(2)), Mul2(Integer(2), Add2(Integer(1), Pow(Tan(Mul2(Symbol(x), Integer(2))), Integer(2)))))

Lambdify

.. code:: python

   >>> lmb = ns.Lambdify([x, y], expr)
   >>> lmb([1.3, 0.2])
   array([[ 0.65359661]])


License
-------
The source code is Open Source and is released under the simplified 2-clause BSD license. See `LICENSE <LICENSE>`_ for further details.

Contributors are welcome to suggest improvements at https://github.com/bjodah/symcxx

Author
------
Björn I. Dahlgren, contact:

- gmail address: bjodah
- kth.se address: bda

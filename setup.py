#!/usr/bin/env python
# -*- coding: utf-8 -*-

import hashlib
import os
import shutil
import sys
from setuptools import setup, Extension


pkg_name = 'symcxx'

SYMCXX_RELEASE_VERSION = os.environ.get('SYMCXX_RELEASE_VERSION', '')  # v*

# Cythonize .pyx file if it exists (not in source distribution)
ext_modules = []


def _read(path, macro='SYMCXX_TYPE', inc_dir='./'):
    for l in open(inc_dir + path, 'rt'):
        if l.startswith('#include'):
            inner_path = l.split('#include')[1]
            inner_path = inner_path.strip().strip('<').strip('>').strip('"')
            for inner_l in _read(inner_path, macro, inc_dir):
                yield inner_l
        if not l.startswith(macro + '('):
            continue
        l = l.split('//')[0]  # strip comments marked with // (misses /* */)
        l = ')'.join(l.split(macro + '(')[1].split(')')[:-1])
        yield tuple([_.strip() for _ in l.split(',')])

if len(sys.argv) > 1 and '--help' not in sys.argv[1:] and sys.argv[1] not in (
        '--help-commands', 'egg_info', 'clean', '--version'):
    try:
        from mako.template import Template
        from mako.exceptions import text_error_template
        from Cython.Build import cythonize
    except ImportError:
        USE_CYTHON = False
    else:
        pyx_path = 'symcxx/_symcxx.pyx'
        template_path = pyx_path + '.mako_template'
        USE_CYTHON = os.path.exists(template_path)

    ext = '.pyx' if USE_CYTHON else '.cpp'
    ext_modules = [Extension(
        'symcxx._symcxx',
        ['symcxx/_symcxx'+ext]
    )]
    if USE_CYTHON:
        stub = 'types_nonatomic_'
        path_stub = 'symcxx/' + stub
        subsd = {}
        subsd['types'] = list(_read('symcxx/types_atomic.inc', inc_dir='./include/'))
        for k in ('unary', 'binary', 'args_stack'):
            subsd[stub+k] = list(_read(path_stub+k+'.inc', inc_dir='./include/'))
            subsd['types'] += subsd[stub+k]

        subsd['_message_for_rendered'] = 'THIS IS A GENERATED FILE DO NOT EDIT'
        try:
            rendered_pyx = Template(open(template_path, 'rt').read()).render(**subsd)
        except:
            print(text_error_template().render_unicode())
            raise
        else:
            sha256hex = hashlib.sha256(rendered_pyx.encode('utf-8')).hexdigest()
            hash_path = os.path.join('build', pyx_path.replace('/', '__')+'.sha256hex')
            if os.path.exists(hash_path) and open(hash_path, 'rt').read(256//4) == sha256hex:
                pass
            else:
                open(pyx_path, 'wt').write(rendered_pyx)
                open(hash_path, 'wt').write(sha256hex)
        ext_modules = cythonize(ext_modules,
                                include_path=['./include'],
                                gdb_debug=True)
    else:
        ext_modules[0].sources = [
            'src/basic.cpp', 'src/namespace.cpp'
        ] + ext_modules[0].sources
    ext_modules[0].include_dirs += ['./include']
    ext_modules[0].language = 'c++'
    ext_modules[0].extra_compile_args = ['-std=c++11']


# http://conda.pydata.org/docs/build.html#environment-variables-set-during-the-build-process
if os.environ.get('CONDA_BUILD', '0') == '1':
    try:
        SYMCXX_RELEASE_VERSION = 'v' + open(
            '__conda_version__.txt', 'rt').readline().rstrip()
    except IOError:
        pass

release_py_path = os.path.join(pkg_name, '_release.py')

if len(SYMCXX_RELEASE_VERSION) > 0:
    if SYMCXX_RELEASE_VERSION[0] == 'v':
        TAGGED_RELEASE = True
        __version__ = SYMCXX_RELEASE_VERSION[1:]
    else:
        raise ValueError("Ill formated version")
else:
    TAGGED_RELEASE = False
    # read __version__ attribute from _release.py:
    exec(open(release_py_path).read())


classifiers = [
    "Development Status :: 3 - Alpha",
    'License :: OSI Approved :: BSD License',
    'Operating System :: OS Independent',
    'Topic :: Scientific/Engineering',
    'Topic :: Scientific/Engineering :: Mathematics',
]

tests = [
    'symcxx.tests',
]

long_description = open('README.rst').read()
with open(os.path.join(pkg_name, '__init__.py')) as f:
    short_description = f.read().split('"""')[1]

setup_kwargs = dict(
    name=pkg_name,
    version=__version__,
    description=short_description,
    long_description=long_description,
    classifiers=classifiers,
    author='Björn Dahlgren',
    author_email='bjodah@DELETEMEgmail.com',
    url='https://github.com/bjodah/' + pkg_name,
    license='BSD',
    packages=[pkg_name] + tests,
    setup_requires=['mako'],
    install_requires=['numpy'],
    ext_modules=ext_modules,
)

if __name__ == '__main__':
    try:
        if TAGGED_RELEASE:
            # Same commit should generate different sdist
            # depending on tagged version (set $SYMCXX_RELEASE_VERSION)
            # e.g.:  $ SYMCXX_RELEASE_VERSION=v1.2.3 python setup.py sdist
            # this will ensure source distributions contain the correct version
            shutil.move(release_py_path, release_py_path+'__temp__')
            open(release_py_path, 'wt').write(
                "__version__ = '{}'\n".format(__version__))
        setup(**setup_kwargs)
    finally:
        if TAGGED_RELEASE:
            shutil.move(release_py_path+'__temp__', release_py_path)

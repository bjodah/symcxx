#!/bin/bash -xeu
if [[ "$CI_BRANCH" =~ ^v[0-9]+.[0-9]?* ]]; then
    eval export ${1^^}_RELEASE_VERSION=\$CI_BRANCH
    echo ${CI_BRANCH} | tail -c +2 > __conda_version__.txt
fi
python2 -m pip install --ignore-installed https://github.com/cython/cython/archive/0.27b1.tar.gz --install-option="--no-cython-compile"
python2 setup.py sdist
ORIPATH=$(pwd)
(cd /; python2 -m pip install $ORIPATH/dist/*.tar.gz)
(cd /; python2 -m pytest --pyargs $1)
python2 setup.py build_ext -i
PYTHONPATH=$ORIPATH PYTHON=python3 CFLAGS='-flto' ./scripts/run_tests.sh --cov $1 --cov-report html
./scripts/coverage_badge.py htmlcov/ htmlcov/coverage.svg

#!/bin/bash -xeu
if [[ "$CI_BRANCH" =~ ^v[0-9]+.[0-9]?* ]]; then
    eval export ${1^^}_RELEASE_VERSION=\$CI_BRANCH
    echo ${CI_BRANCH} | tail -c +2 > __conda_version__.txt
fi
python2 -m pip install --upgrade --pre cython
python2 setup.py sdist
pip install dist/*.tar.gz
(cd /; python2 -m pytest --pyargs $1)
python2 setup.py build_ext -i
CFLAGS='-flto' python3 setup.py build_ext -i
python3 -m pytest
for PYTHON in python2 python3; do
    PYTHONPATH=$(pwd) ./scripts/run_tests.sh --cov $1 --cov-report html
done
./scripts/coverage_badge.py htmlcov/ htmlcov/coverage.svg
! grep "DO-NOT-MERGE!" -R . --exclude ci.sh

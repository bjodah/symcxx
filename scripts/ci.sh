#!/bin/bash -xeu
if [[ "$DRONE_BRANCH" =~ ^v[0-9]+.[0-9]?* ]]; then
    eval export ${1^^}_RELEASE_VERSION=\$CI_BRANCH
fi
python3 setup.py sdist
ORIPATH=$(pwd)
(cd /; python3 -m pip install $ORIPATH/dist/*.tar.gz)
(cd /; python3 -m pytest --pyargs $1)
python3 setup.py build_ext -i
PYTHONPATH=$ORIPATH PYTHON=python3 ./scripts/run_tests.sh --cov $1 --cov-report html
./scripts/coverage_badge.py htmlcov/ htmlcov/coverage.svg

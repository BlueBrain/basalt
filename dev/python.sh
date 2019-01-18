#!/bin/bash -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

if ! ${FLAKE8:-flake8} --version >/dev/null 2>&1 ; then
    echo "flake8 is required. Override name with FLAKE8 env var" >&2
    exit 1
fi

if ! ${BLACK:-black} --version >/dev/null 2>&1 ; then
    echo "black is required. Override name with BLACK env var" >&2
    exit 1
fi

${BLACK:-black} -S setup.py basalt tests
${FLAKE8:-flake8} basalt tests

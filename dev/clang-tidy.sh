#!/bin/bash -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

if ! ${CLANG_TIDY:-clang-tidy} --version >/dev/null 2>&1 ; then
    echo "clang-tidy is required. Override name with CLANG_TIDY env var" >&2
    exit 1
fi

find $SCRIPT_DIR/../src/basalt -name \*cpp -print0 |
  xargs -0 -n 1 -I '{}' ${CLANG_TIDY:-clang-tidy} '{}' -- \
    -I $SCRIPT_DIR/../include \
    -I $SCRIPT_DIR/../src/third_party/spdlog/include \
    \;

#!/bin/bash -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

if ! ${CLANG_TIDY:-clang-tidy} --version >/dev/null 2>&1 ; then
    echo "clang-tidy is required. Override name with CLANG_TIDY env var"
    exit 1
fi	

${CLANG_TIDY:-clang-tidy} $SCRIPT_DIR/../src/basalt/network.cc -- \
  -I $SCRIPT_DIR/../include \
  -I $SCRIPT_DIR/../src/third_party/spdlog/include

#!/bin/bash -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

if ! ${CLANG_FORMAT:-clang-format} --version >/dev/null 2>&1 ; then
    echo "clang-format is required. Override name with CLANG_FORMAT env var" >&2
    exit 1
fi

find $SCRIPT_DIR/../include $SCRIPT_DIR/../src/basalt $SCRIPT_DIR/../test \
  -type f \
  \( -name "*.hpp" -o -name "*.hxx" -o -name "*.cpp" \) \
  -exec ${CLANG_FORMAT:-clang-format} -i \{\} \;

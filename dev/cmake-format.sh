#!/bin/bash -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

if ! ${CMAKE_FORMAT:-cmake-format} --version >/dev/null 2>&1 ; then
    echo "cmake-format is required. Override name with CMAKE_FORMAT env var" >&2
    exit 1
fi

find $SCRIPT_DIR/.. -path $SCRIPT_DIR/../src/third_party -prune \
  -type f -name CMakeLists.txt \
  -exec ${CMAKE_FORMAT:-cmake-format} -i \{\} \;

find $SCRIPT_DIR/../cmake -type f -name "*.cmake" \
  -exec ${CMAKE_FORMAT:-cmake-format} -i \{\} \;

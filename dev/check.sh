#!/bin/bash -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

for checker in clang-format clang-tidy cmake-format; do
	$SCRIPT_DIR/$checker.sh
done

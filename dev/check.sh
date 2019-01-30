#!/bin/bash -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"

for checker in clang-tidy python; do
	$SCRIPT_DIR/$checker.sh
done

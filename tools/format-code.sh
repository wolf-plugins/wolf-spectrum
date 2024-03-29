#!/usr/bin/env bash

# SPDX-FileCopyrightText: © 2023 Patrick Desaulniers
# SPDX-License-Identifier: GPL-3.0-or-later

# This script formats all the cpp/hpp files in a specific directory using clang-format.

set -e
shopt -s globstar

if [ $# -ne 1 ]; then
	echo "Invalid number of arguments."
        echo "Usage: format-code.sh <dir>"
	exit 1
fi

for cfile in "$1"/**/*.{cpp,hpp}; do
	echo "Formatting $cfile"
	clang-format -style=file -i "$cfile" 
done

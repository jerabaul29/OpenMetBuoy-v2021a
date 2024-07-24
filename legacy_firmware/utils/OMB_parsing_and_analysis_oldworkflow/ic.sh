#! /usr/bin/env bash

# Copyright (c) 2021 Jesús Lázaro
# 
# This software is released under the MIT License.
# https://opensource.org/licenses/MIT


# print file, function name, line, variable and its value
ic () {
	local tmp="${1}[@]"
	echo "(${BASH_SOURCE[1]},${FUNCNAME[1]}) ${BASH_LINENO[0]}: $1 - ${!tmp}"
}

# print file, function name, line, and message string
icp () {
	echo "(${BASH_SOURCE[1]},${FUNCNAME[1]}) ${BASH_LINENO[0]}: $1"
}

# print full call tree and variable and its value
ict () {
	local tmp="${1}[@]"
	for((i=${#BASH_SOURCE[@]}-2;i>=0;i--));
	do
		echo -n "(${BASH_SOURCE[$i+1]},${FUNCNAME[$i+1]}) ${BASH_LINENO[$i]}: "
	done
	echo "$1 - ${!tmp}"
}

# print full call tree and message string
ictp () {
	for((i=${#BASH_SOURCE[@]}-2;i>=0;i--));
	do
		echo -n "(${BASH_SOURCE[$i+1]},${FUNCNAME[$i+1]}) ${BASH_LINENO[$i]}: "
	done
	echo "$1"
}


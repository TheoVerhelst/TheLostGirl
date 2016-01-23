#!/bin/bash
# Profiling script for The Lost Girl.
#
# This script assumes that an executable called TheLostGirl is located
# in the current directory, and a profiling output called gmon.out is
# also located in the current directory. This script also use python3
# for the truncating feature.
# 
# The executable have to be compiled with the option -pg if this is
# g++, or any other option that makes a profile output suitable for
# use with gprof.
#
# The trucating option truncate the output file such that only 145
# characters are displayed on the screen. This is more readable when
# very long template names are used.
#

print_usage()
{
	echo "Usage: $0 [-t] PATH"
	exit 1
}

truncate=`false`
path=$1

if [[ $# -gt 2 || $# -eq 0 || ( $# -eq 2 && $1 != "-t" ) ]]
then
	print_usage
elif [[ $# -eq 2 ]]
then
	truncate=`true`
	path=$2
fi

tmp=`mktemp`
gprof "$path/TheLostGirl" "$path/gmon.out" > "$tmp"
if $truncate
then
	python3 -c "
lines = []
with open('$tmp') as file:
	for line in file:
		line = line[:145]
		if line[-1] != '\n':
			line += '\n'
		lines.append(line)
with open('$tmp', 'w') as file:
	file.write(''.join(lines))"
fi
less -p"Call graph" $tmp
rm $tmp

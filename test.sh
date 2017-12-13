#!/bin/bash
EX=compiler

make
if [ $# -gt 0 ]
then
	./$EX < $1; printf "\n________\n" ;  cat latexout
else
	for f in TestCases/*; do
		name="${f##*/}"
		./$EX < "$f"
		cat latexout > TestOutput/"${name%.tst}".out
	done
    clear
    printf "All files in TestCases/ have been successfully compiled and stored in TestOutput/\n"
fi

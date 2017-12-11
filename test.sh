#!/bin/bash
EX=compiler

make
if [ $# -gt 0 ]
then
	./$EX < $1; printf "\n________\n" ;  cat latexout
fi
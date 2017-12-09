#!/bin/bash
EX=compiler

if [ $# -gt 0 ]
then
	./$EX < $1; printf "\n________\n" ;  cat latexout
fi
#!/bin/bash
rm stdout
rm stderr

for im in *.tex
do
	echo "****** processing $im" 1>>stdout
	echo "****** processing $im" 2>>stderr
	pdflatex --shell-escape $im 1>>stdout 2>>stderr
done



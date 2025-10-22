#!/bin/sh

lhs=$1
tex=$2

lhs2TeX "$1" > "$2"
pdflatex --synctex=1 "$2"

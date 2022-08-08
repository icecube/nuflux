#!/bin/sh
rm test_inline_readme
awk '/END CODE/ {p=0};p;/BEGIN CODE/ {p=1}' ../README.md | g++ -lnuflux -x c++ -o test_inline_readme - 
f=`./test_inline_readme`
echo $f
if [ "$f" = '4.64762e-11' ]; then 
    echo "Correct Flux"
    exit 0
else
    echo "incorrect Flux"
    exit -1
fi;

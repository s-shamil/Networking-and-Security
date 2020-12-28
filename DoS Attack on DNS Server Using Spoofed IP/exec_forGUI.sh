#!/bin/bash
g++ mycode.cpp -o mycode

for (( i=1; i<=120; i++ ))
do
	./mycode $1 $2 2 500
	echo "$i"
done

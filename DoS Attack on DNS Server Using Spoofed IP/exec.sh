#!/bin/bash
g++ dns_maker.cpp -o dns_maker

for (( i=1; i<=12000; i++ ))
do
	./dns_maker 192.168.43.110 1 2 500
	echo "$i"
done

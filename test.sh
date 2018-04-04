#!/bin/bash

echo "open tests"
for i in `seq 0 10`;
do
	./harness open $i $i
done

echo "fork tests"
for j in `seq 0 10`;
do
        ./harness fork $j $j
done

echo "open: no arguments"
./harness open

echo "open: one argument"
./harness open 1

echo "open: 3 arguments"
./harness open 1

echo "open: non-number arguments"
./harness open hi hello

echo "fork: no arguments"
./harness fork

echo "fork: one argument"
./harness fork 1

echo "fork: 3 arguments"
./harness fork 1

echo "fork: non-number arguments"
./harness fork hi hello

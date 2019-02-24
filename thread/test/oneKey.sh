#!/bin/bash

for (( i=1; i<20; i++ ))
do
    echo "N=${i}" > input.txt
    echo "M=4294967295" >> input.txt
    ./multisum 
done



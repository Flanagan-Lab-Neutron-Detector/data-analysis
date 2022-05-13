#!/bin/bash

start=$1
stop=$2
step=$3

echo "This will take a while. Maybe 1/2 hour depending on how much data you have. I would recommend running with &"
echo "Also be sure you run this script from a directory full of data files"
echo "This script takes three new numeric arguments, start, stop (exclusive), and step for voltage."

echo "voltage #0's" > diagnostic.txt
for (( v = start; v < stop; v = v+step ))
do
    echo -n "$v "
    grep -o 1 *-${v}-* | wc -l
done >> diagnostic.txt

INITIAL_WORKING_DIRECTORY=$(pwd)
cd "$(dirname "$0")"

python3 plot-diagnostic.py $INITIAL_WORKING_DIRECTORY/diagnostic.txt

cd $INITIAL_WORKING_DIRECTORY

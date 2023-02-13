#!/bin/bash

v=$1

echo "Processing ${v}"
echo -n "$v " >> diagnostic_${v}.txt
grep -o 1 *-${v}-* | wc -l >> diagnostic_${v}.txt
echo "Finished ${v}"

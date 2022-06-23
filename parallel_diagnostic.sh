#!/bin/bash

if [[ $0 == "bash" ]]; then
	echo "It appears that this script has been sourced (\$0 == 'bash'). Please run this script as an executable." 1>&2
	exit 2
fi

if [[ $# -lt 3 ]]; then
	echo "Not enough arguments." 1>&2
	echo "Usage: parallel_diagnostic.sh start stop step" 1>&2
	exit 1
fi

vstart=$1
vstop=$2
vstep=$3

# the first if hopefully ensures we can do this
script_dir=$(dirname $0)

echo "Processing ${vstart} to ${vstop} (inclusive) in steps of ${vstep}"

# Seq will generate the full voltage sequence, which we feed to parallel, which will pass each one
# to one invocation of simple_diagnostic.sh

seq $vstart $vstep $vstop | parallel --lb $script_dir/simple_diagnostic.sh

if [[ $? -ne 0 ]]; then
	echo "Error. Parallel returned $0" 1>&2
	rm diagnostic*.txt
	exit 3
fi

# files will be emitted named diagnostic_[voltage].txt
# we need to concatenate them in order, then run the python script to generate a plot

for (( v = vstart; v <= vstop; v = v+vstep ))
do
	cat diagnostic_${v}.txt >> diagnostic.txt
	rm diagnostic_${v}.txt
done

cat diagnostic.txt | tail -n +2 | gnuplot -e "set terminal dumb; set nokey; plot '<cat'"

#python3 $script_dir/plot-diagnostic.py ./diagnostic.txt

echo "Done"

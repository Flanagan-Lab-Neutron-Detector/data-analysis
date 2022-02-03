# data-analysis
This repository contains data analysis code for producing beam profiles and heatmaps using root and C++

## 2020-2021 C code for the Cerium chip

### Heatmap.C
Reads a CSV file, builds a TTree, and prints output to a histogram. Includes fit functions and projections

### Part Numbers
These are the part numbers for all the chips:

37	47	57	62
73	79	82	85
91	98	100	103
110	114	115	125

The cadmium part is 109.

## 2021-present python code for the Infineon chip

### data-format-CLI
This macro converts bitmap files for each reference voltage and sector to a single voltage map.
Important notes:
1. The voltage given is the voltage at which the bit readout transitions from a zero to a one. If this occurs multiple times, it is the last such voltage. The voltage given is the voltage where the one was read, not the zero. If no such transition occurs, the voltage will read 0.
2. Other transitions (such as, in bad data, from 1 to 2 or 2 to 1) are ignored.


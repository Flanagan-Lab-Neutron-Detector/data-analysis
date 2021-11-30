import sys
import os
import numpy as np
from argparse import ArgumentParser


def get_bits(location, fname):
    return np.reshape(np.genfromtxt(os.path.join(location, fname), delimiter=1), -1)

def get_sector_data(location, fbase, sector, vstart, vstop, vstep):
    bits = np.zeros(2**20,dtype=int)
    data = np.zeros(2**20,dtype=int)
    # loop over voltages
    for voltage in range(vstart, vstop, vstep):
        new_bits = get_bits(location, fbase.format(voltage,sector))
        for i,bit in enumerate(bits):
            if bit != new_bits[i]:
                data[i] = voltage
        bits = new_bits
    return data

def write_sector_data(location, fbase, sector, data):
    fname = fbase.format(sector)
    np.savetxt(os.path.join(location, fname), data, fmt="%d", delimiter=",")
    return

def convert_files(input_directory, basename, output_directory, filename, vstart, vstop, vstep):
    for sector in range(0, 2**26, 2**16):
        data = get_sector_data(input_directory, basename, sector, vstart, vstop, vstep)
        write_sector_data(output_directory, filename, sector, data)

# CLI spec
parser = ArgumentParser(description="CLI for converting bitwise data files to csv voltage files")

parser.add_argument('-id', '--input-directory', type=str, default='.', help="the directory containing input files")
parser.add_argument('-if', '--basename', type=str, required=True, help="the base file name. replace voltage and address with '{}', i.e. TEST1-{}-{}")
parser.add_argument('-od', '--output-directory', type=str, default='.', help="the directory containing output files")
parser.add_argument('-of', '--filename', type=str, default='CHIP_VOLTAGES_{}', help="the base output file name. replace address with {}, i.e. CHIP_VOLTAGES_{}")
parser.add_argument('--start', type=int, required=True, help='the lowest voltage in mV')
parser.add_argument('--stop', type=int, required=True, help='the highest voltage (plus the step) in mV')
parser.add_argument('--step', type=int, required=True, help='the granularity in mV')

args = parser.parse_args()

convert_files(args.input_directory, args.basename, args.output_directory, args.filename, args.start, args.stop, args.step)

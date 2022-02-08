import sys
import os
import numpy as np
from argparse import ArgumentParser
import time

# Helper function for displaying time prettily
def formatted_time(t):
    t = int(t)
    s = t % 60
    t //= 60
    m = t % 60
    t //= 60
    h = t % 24
    t //= 24
    d = t % 365
    t //= 365
    y = t
    return f"{y} years {d} days" if y else f"{d} days {h} hours" if d else f"{h} hours {m} minutes" if h else f"{m} minutes {s} seconds" if m else f"{s} seconds"

# Totally unnecessary progress meter ripped from the bowels of the internet
# Print iterations progress
def printProgressBar(iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
    global progBarStartTime
    """
    Call in a loop to create terminal progress bar
    @params:
        iteration   - Required  : current iteration (Int)
        total       - Required  : total iterations (Int)
        prefix      - Optional  : prefix string (Str)
        suffix      - Optional  : suffix string (Str)
        decimals    - Optional  : positive number of decimals in percent complete (Int)
        length      - Optional  : character length of bar (Int)
        fill        - Optional  : bar fill character (Str)
        printEnd    - Optional  : end character (e.g. "\r", "\r\n") (Str)
    """
    if iteration == 0:
        progBarStartTime = time.time()
        remainingTime = 0 #nobody cares
    else:
        elapsedTime = int(time.time() - progBarStartTime)
        totalTime = int(elapsedTime * total / (iteration+1))
        remainingTime = totalTime - elapsedTime
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)
    print(f'\r{prefix} |{bar}| {percent}% {suffix} / {formatted_time(remainingTime)} remaining        ', end = printEnd)
    # Print New Line on Complete
    if iteration == total: 
        print()

def get_bits(location, fname):
    return np.reshape(np.genfromtxt(os.path.join(location, fname), delimiter=1), -1)

def write_sector_data(location, fbase, sector, data):
    fname = fbase.format(sector)
    np.savetxt(os.path.join(location, fname), data, fmt="%d", delimiter=",")
    return

# read a 1 at min voltage--record min voltage
# read a 0 at max voltage--record voltage above
# read a 1 and then a 0--print a warning, ignore any 1s before the 0. 
def convert_files(input_directory, basename, output_directory, filename, vstart, vstop, vstep, sectors):
    
    printProgressBar(0, ((vstop-vstart)/vstep)*len(sectors), prefix='Converting data files: ', suffix='complete', decimals=0, length=50)
    count = 0
    
    for sector in sectors:
        old_bits = np.zeros(2**20,dtype=int)
        data = np.zeros(2**20,dtype=int)
        # loop over voltages
        for voltage in range(vstart, vstop, vstep):
            bad_bit_count = 0
            new_bits = get_bits(input_directory, basename.format(voltage,sector))
            for i in range(len(old_bits)):
                if old_bits[i] == 0 and new_bits[i] == 1:
                    data[i] = voltage
                elif old_bits[i] == 1 and new_bits[i] == 0:
                    bad_bit_count += 1
                elif voltage == vstop - vstep and old_bits[i] == 0:
                    data[i] = vstop
            old_bits = new_bits

            if bad_bit_count > 0:
                print(f"Warning: {bad_bit_count} bits were caught flipping in the wrong direction. These bad flips will be ignored.")
            
            printProgressBar(count, ((vstop-vstart)/vstep)*len(sectors), prefix='Converting data files: ', suffix='complete', decimals=0, length=50)
            count+=1
        write_sector_data(output_directory, filename, sector, data)

# CLI spec
parser = ArgumentParser(description="CLI for converting bitwise data files to csv voltage files")

parser.add_argument('-id', '--input-directory', type=str, default='.', help="the directory containing input files")
parser.add_argument('-if', '--basename', type=str, required=True, help="the base file name. replace voltage and address with '{}', i.e. TEST1-{}-{}")
parser.add_argument('-od', '--output-directory', type=str, default='.', help="the directory containing output files")
parser.add_argument('-of', '--filename', type=str, default='CHIP_VOLTAGES_{}.csv', help="the base output file name. replace address with {}, i.e. CHIP_VOLTAGES_{}.csv")
parser.add_argument('--start', type=int, required=True, help='the lowest voltage in mV')
parser.add_argument('--stop', type=int, required=True, help='the highest voltage (plus the step) in mV')
parser.add_argument('--step', type=int, required=True, help='the granularity in mV')
parser.add_argument('--sectors', type=int, nargs='+', help='the sector numbers to process')
parser.add_argument('--all-sectors', action='store_true', help='read entire chip')
parser.add_argument('--last-sector', type=int, required=False, default=(2**26-2**16), help='the sector number of the last sector to attempt reading--use in cases of incomplete data')

args = parser.parse_args()


if not args.sectors or args.all_sectors:
    parser.error("--sectors or --all-sectors is required")
if args.sectors and args.all_sectors:
    parser.error("--sectors and --all_sectors are incompatible")

os.mkdir(args.output_directory)

if args.all_sectors:
    sectors = range(0, args.last_sector+2**16, 2**16)
else:
    sectors = args.sectors
convert_files(args.input_directory, args.basename, args.output_directory, args.filename, args.start, args.stop, args.step, args.sectors)

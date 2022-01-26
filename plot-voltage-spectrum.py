import matplotlib.pyplot as plt
import numpy as np
from argparse import ArgumentParser

def plot_voltages(inputfile, outputfile, title):
    data = np.zeros(2**20, dtype=int)
    with(open(inputfile)) as f:
        for i, v in enumerate(f):
            data[i] = v
    plt.title(title)
    plt.hist(data)
    plt.savefig(outputfile)
    plt.show()
    

# CLI spec                                                                                                                                                                                                                                                                        
parser = ArgumentParser(description="CLI for making voltage histograms")

parser.add_argument('-if', '--input-file', type=str, required=True, help="a file containing line-separated voltages only")
parser.add_argument('-of', '--output-file', type=str, default='voltage-spectrum.png', help="the name of the output file containing the plot")
parser.add_argument('-t', '--title', type=str, default='Voltage spectrum', help="the title for the plot")
#parser.add_argument('-of', '--filename', type=str, default='CHIP_VOLTAGES_{}', help="the base output file name. replace address with {}, i.e. CHIP_VOLTAGES_{}")
#parser.add_argument('--start', type=int, required=True, help='the lowest voltage in mV')
#parser.add_argument('--stop', type=int, required=True, help='the highest voltage (plus the step) in mV')
#parser.add_argument('--step', type=int, required=True, help='the granularity in mV')
#parser.add_argument('--last-sector', type=int, required=False, default=(2**26-2**16), help='the sector number of the last sector to attempt reading--use in cases of incopmlete data')

args = parser.parse_args()

plot_voltages(args.input_file, args.output_file, args.title)




import matplotlib.pyplot as plt
import numpy as np
from argparse import ArgumentParser
import os


def plot_voltages(inputdir, outputfile, title, verbose):
    data = []
    for inputfile in os.scandir(path=inputdir):
        if inputfile.is_file() and inputfile.name[-4:] == '.csv':
            if(verbose):
                print(f"Reading data from file {inputfile.name}")
            with(open(inputfile.path)) as f:
                for v in f:
                    data.append(int(v))
    data = np.asarray(data)
    plt.title(title)
    plt.hist(data, bins=20)
    plt.savefig(outputfile)
    plt.show()
    

# CLI spec                                                                                                                                                                                                                                                                        
parser = ArgumentParser(description="CLI for making voltage histograms")

parser.add_argument('-id', '--input-dir', type=str, required=True, help="a folder in which all csv files contain line-separated voltages only")
parser.add_argument('-of', '--output-file', type=str, default='voltage-spectrum.png', help="the name of the output file containing the plot")
parser.add_argument('-t', '--title', type=str, default='Voltage spectrum', help="the title for the plot")
parser.add_argument('-v', '--verbose', action='store_true', help='print more information')

args = parser.parse_args()

plot_voltages(args.input_dir, args.output_file, args.title, args.verbose)


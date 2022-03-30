import matplotlib.pyplot as plt
import numpy as np
from argparse import ArgumentParser
import os
import probscale


def get_voltages(inputdir, verbose):
    data = []
    for inputfile in os.scandir(path=inputdir):
        if inputfile.is_file() and inputfile.name[-4:] == '.csv':
            if(verbose):
                print(f"Reading data from file {inputfile.name}")
            with(open(inputfile.path)) as f:                
                for v in f:
                    data.append(int(v))
    return np.asarray(data)
    
def get_deltas(inputdir, inputdir2, verbose):
    data = []
    for inputfile in os.scandir(path=inputdir):
        if inputfile.is_file() and inputfile.name[-4:] == '.csv':
            if(verbose):
                print(f"Reading data from file {inputfile.name}")
            with(open(inputfile.path)) as f:
                with(open(os.path.join(inputdir2, inputfile.name))) as f2:
                    for v in f:
                        data.append(int(next(f2)) - int(v))
    return np.asarray(data)
    
def plot_histogram(data, outputfile, title, hide):
    plt.title('Voltage Distribution' if title == 'title' else title)
    plt.hist(data, bins=20)
    plt.savefig(outputfile)
    if not hide:
        plt.show()

def plot_probability(data, outputfile, title, hide):
    fig, ax = plt.subplots()
    fig = probscale.probplot(data, ax=ax, plottype='qq', probax='y', datascale='log', problabel="$\sigma$", datalabel="Voltage", scatter_kws=dict(marker='.', linestyle='none', label=''))
    plt.title('Probability plot' if title == 'title' else title)
    plt.savefig(outputfile)
    if not hide:
        plt.show()

    
# CLI spec                                                                                                                                                                                                                                                                        
parser = ArgumentParser(description="CLI for making voltage histograms")

parser.add_argument('-id', '--input-dir', type=str, required=True, help="a folder in which all csv files contain line-separated voltages only")
parser.add_argument('-id2', '--input-dir-2', type=str, help="a folder containing csv files with identical names as --input-dir, to compare voltages")
parser.add_argument('-of', '--output-file', type=str, default='voltage-plot.png', help="the name of the output file containing the plot")
parser.add_argument('-t', '--title', type=str, default='title', help="the title for the plot")
parser.add_argument('-v', '--verbose', action='store_true', help='print more information')
parser.add_argument('-p', '--probability-plot', action='store_true', help='create a probability plot instead of a histogram')
parser.add_argument('--hide-plots', action='store_true', help='do not call plt.show after creating the plot')

args = parser.parse_args()

if args.input_dir_2:
    data = get_deltas(args.input_dir, args.input_dir_2, args.verbose)
else:
    data = get_voltages(args.input_dir, args.verbose)

if args.probability_plot:
    plot_probability(data, args.output_file, args.title, args.hide_plots)
else:
    plot_histogram(data, args.output_file, args.title, args.hide_plots)

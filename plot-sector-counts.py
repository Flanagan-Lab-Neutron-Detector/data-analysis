import numpy as np
import matplotlib.pyplot as plt
from sys import argv
from os import listdir

directory_name = argv[1]
print("Scanning directory", directory_name)
for fname in listdir(directory_name):
	print(fname)

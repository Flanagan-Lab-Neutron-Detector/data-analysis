import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
import re
from sys import argv
from os import listdir, path

hexint = lambda x: int(x, 16)

mv_counts = []

directory_name = argv[1]
#print("Scanning directory", directory_name)
regex = re.compile(r'counts-(\d\d\d\d).csv')
for fname in listdir(directory_name):
	m = regex.fullmatch(fname)
	if m:
		fpath = path.join(directory_name, fname)
		mv = int(m.groups(1)[0])
		#print("Found", mv, "mV")
		data = np.loadtxt(fpath, dtype=(int, int), delimiter=',', skiprows=1, converters={0:hexint, 1:int})
		mv_counts.append((mv, data))

mv_counts = sorted(mv_counts)

mv_list = []
for i in range(len(mv_counts)):
	mv_list.append(mv_counts[i][0])
sector_count = len(mv_counts[0][1])

header = "Sector"
for mv in mv_list:
	header += f", {mv:d}mV"
print(header)

for sector in range(0, sector_count):
	line = f"{sector:d}"
	for i in range(len(mv_counts)):
		mv = mv_counts[i][0]
		counts = mv_counts[i][1][sector][1]
		line += f", {counts:d}"
	print(line)

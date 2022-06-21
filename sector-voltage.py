import numpy as np
#from argparse import ArgumentParser
import sys
import os
import matplotlib.pyplot as plt
from pathlib import Path

def auto_int(x):
	return int(x, 0)

def select_voltage(word_values, voltage_list):
	"""
	Selects a voltage from a list of voltages, for an entire word
	"""
	voltages = [voltage_list[-1]] * 16
	for bit in range(16):
		# scan from low to high, we choose the first voltage at which the bit is set
		for i,w in enumerate(word_values):
			if ((w >> bit) & 1) == 1:
				voltages[bit] = voltage_list[i]
				break
	return voltages

sector = auto_int(sys.argv[1])
print(f"Searching for Sector 0x{sector:x} ({sector:d})")

voltages = {}

for fname_raw in os.listdir():
	fname = Path(fname_raw)
	basename = fname.stem
	comp = basename.split('-')
	if len(comp) == 3 and comp[2] == f"{sector:d}":
		voltages[int(comp[1])] = np.loadtxt(fname_raw, dtype=np.dtype("<H"), converters={0:lambda s: int(s,2)})

# get all the voltages

mv_values = sorted(voltages.keys())
sector_bit_voltages = np.zeros((64*1024, 16), dtype=int)

for word_index in range(64*1024):
	if (word_index % 1024) == 0:
		print(f"{100.0*(word_index/1024.0)/64.0}%")
	word_values = [voltages[v][word_index] for v in mv_values]
	word_voltages = select_voltage(word_values, mv_values)
	sector_bit_voltages[word_index] = word_voltages
	#for i,v in enumerate(word_voltages):
		#sector_bit_voltages[word_index][i] = v

sv = sector_bit_voltages/1000.0
print(sv)

word_avg = sv.mean(axis=1)

page_avg = sv.reshape(-1, 8, sv.shape[1]).mean(axis=1)

plt.title(f"Mean of bit voltages of all words in sector 0x{sector:X}")
plt.plot(word_avg[:240])
for x in range(0, 240, 8):
	plt.plot([x, x], [6.0, 6.5], 'k--')
plt.show()

#fig, ax = plt.subplots(2)
#fig.suptitle(f"First 16 words of sector 0x{sector:X}")

#ax[0].imshow(sv[:16])

#ax[1].plot(range(16), word_avg[:16])

#ax[1].plot(range(16), page_avg[0], 'k')
#ax[1].plot(range(16), page_avg[1], 'b')

#for i in range(8):
	#ax[1].plot(range(16), sv[i], 'k', label=f"w{i:x}")
#for i in range(8, 16):
	#ax[1].plot(range(16), sv[i], 'b', label=f"w{i:x}")

plt.show()

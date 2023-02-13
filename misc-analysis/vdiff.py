import matplotlib.pyplot as plt
import numpy as np
from sys import argv
from pathlib import Path

if len(argv) < 4:
    print(f"Usage: {argv[0]} chipname prefile postfile")
    exit(0)

chipname = argv[1]
prepath = Path(argv[2])
postpath = Path(argv[3])

bins = range(3900, 7100, 100)

# load and plot pre and post

print(f"1/2 Loading {str(prepath)}...", end='', flush=True)
predata = np.fromfile(prepath, dtype=np.int16)
print(" plotting...", end='', flush=True)
plt.hist(predata, histtype='step', bins=bins, label=f"{chipname} Pre")
print(" done.")

print(f"2/2 Loading {str(postpath)}...", end='', flush=True)
postdata = np.fromfile(postpath, dtype=np.int16)
print(" plotting...", end='', flush=True)
plt.hist(postdata, histtype='step', bins=bins, label=f"{chipname} Post")
print(" done.")

plt.xlabel("$V_{WL}$ [mV]")
plt.ylabel("Count")
plt.title(f"{chipname} Pre and Post Bit Voltage")
plt.yscale('log')
plt.legend()
plt.show()

# subtract and plot

diffbins = range(-10000, 10100, 100)

print(f"Calculating difference...", end='', flush=True)
diffdata = postdata - predata
print(" plotting...", end='', flush=True)
plt.hist(diffdata, histtype='step', bins=diffbins, label=f"{chipname} $\Delta V$")
print(" done.")

plt.xlabel("$\Delta V$ [mV]")
plt.ylabel("Count")
plt.title(f"{chipname} Change in Bit Voltage")
plt.yscale('log')
plt.show()

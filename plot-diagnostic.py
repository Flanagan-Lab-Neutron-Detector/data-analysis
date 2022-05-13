import matplotlib.pyplot as plt
import numpy as np
import sys
import os

f = sys.argv[1]
print("Filename: " + f)
dfile = np.loadtxt(f, skiprows=1)
dfile = dfile.T

plt.plot(dfile[0][1:],np.diff(dfile[1]))
plt.title("Diagnostic data for " + os.path.dirname(f))
plt.xlabel("Voltage (mV)")
plt.ylabel("Count")
plt.savefig(os.path.join(os.path.dirname(f), "diagnostic.png"))


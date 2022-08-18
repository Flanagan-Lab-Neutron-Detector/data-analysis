import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
import re
from sys import argv
from os import listdir, path
import scipy
from scipy.optimize import curve_fit

hexint = lambda x: int(x, 16)

file_name = argv[1]

# Get header fields
header = ""
with open(file_name) as f:
	header = f.readline()
cols = [s.strip() for s in header.split(',')]

print(cols)
# Transform to mv
mv = [int(c[:4]) for c in cols[1:]] # skip first element (sector number)
print(mv)

# Load data
data = np.loadtxt(file_name, delimiter=',', skiprows=1)

print(len(data), len(data[0]))
print(data)

# color map
sector_count = len(data)
viridis = cm.get_cmap('viridis', sector_count-1)

# Plot all
for sector_data in data:
	xdata = mv
	ydata = sector_data[1:]
	#ydata = [sector_data[1+i] - sector_data[i] for i in range(1, len(sector_data)-1)]
	plt.plot(xdata, ydata, color=viridis(sector_data[0]/(sector_count-1)), label="Sector {sector_data[0]}")
plt.title("Bits vs. Vwl, all sectors")
plt.xlabel("Vwl [mV]")
plt.ylabel("Bits Set")
plt.show()

def erf_fit(x, xoff, xscale, yscale):
	return yscale*(1 + scipy.special.erf(xscale*(x-xoff)))

def norm_cdf_fit(x, xoff, xscale, yscale):
	return yscale * scipy.stats.norm.cdf(xscale * (x - xoff))

def norm_fit(x, mu, sigma, amp):
	return amp * scipy.stats.norm.pdf(x, loc=mu, scale=sigma)

p0 = [6050, 0.004, 2**20]
normp0 = [6100, 200, 250000]

norm_xdata = mv[1:]
norm_ydata = []
for i in range(1, len(data[0][1:])):
	norm_ydata.append(data[0][1+i] - data[0][i])

#popt1, pcov1 = curve_fit(norm_cdf_fit, mv, data[0][1:], p0=p0, maxfev=8000)
popt1, pcov1 = curve_fit(norm_fit, norm_xdata, norm_ydata, p0=normp0, maxfev=8000)
print("[mu, sigma, amplitude] =", popt1)
print(pcov1)
yfit = [norm_fit(x, *popt1) for x in norm_xdata]
plt.plot(norm_xdata, norm_ydata, label="Data")
plt.plot(norm_xdata, yfit, label="Fit")
plt.legend()
plt.xlabel("Vwl [mV]")
plt.ylabel("Bits Set")
plt.title("Fit, Bits Set vs Voltage")
plt.show()

"""
params = []

for sector_data in data:
	xdata = mv[1:]
	ydata = [sector_data[1+i] - sector_data[i] for i in range(1, len(sector_data)-1)]

	#popt, pcov = curve_fit(erf_fit, xdata, ydata, p0=p0, maxfev=8000)
	popt, pcov = curve_fit(norm_fit, xdata, ydata, p0=normp0, maxfev=8000)
	params.append(popt)

#	yfit = [erf_fit(x, *popt) for x in mv]

#	diff = [ydata[i] - yfit[i] for i in range(len(ydata))]
#	plt.plot(mv, diff, color=viridis(sector_data[0]/sector_count), label=f"Fit error, sector {sector_data[0]}")

params = np.array(params)

fig, axs = plt.subplots(3, 1, sharex=True)

axs[0].plot(range(sector_count), params[:,0], label=f"$\mu$")
#axs[0].set_xlabel("Sector")
axs[0].set_ylabel(f"$\mu$ [mV]")

axs[1].plot(range(sector_count), params[:,1], label=f"$\sigma$")
#axs[1].set_xlabel("Sector")
axs[1].set_ylabel(f"$\sigma$ [1/mV]")

axs[2].plot(range(sector_count), params[:,2], label=f"$A$")
axs[2].set_xlabel("Sector")
axs[2].set_ylabel(f"Amplitude [bits]")

axs[0].set_title("Fit parameters by Sector")
plt.show()
"""

#print("popt")
#print(popt)
#print("pcov")
#print(pcov)


#plt.plot(xdata, ydata, label="Data")
#plt.plot(xdata, ydata, label="Fit")
#plt.legend()

#plt.xlabel("Vwl [mV]")
#plt.ylabel("fit error, %")
#plt.title("fit error vs. voltage")
#plt.show()


#for sector_data in data:
	# First element is sector number; skip
#	plt.plot(mv, sector_data[1:], color=viridis(sector_data[0]/sector_count), label=f"Sector {sector_data[0]}")

#count_delta = []
#for i in range(1,len(sector_counts_mv)):
#	count_delta.append(sector_counts_mv[i] - sector_counts_mv[i-1])
#plt.plot(mv_list, sector_counts_mv, color=viridis(sector/sector_count), label=f"Sector {sector}")
#plt.plot(mv_list[0:-1], count_delta, color=viridis(sector/sector_count), label=f"Sector {sector}")

#plt.xlabel("Vwl [mV]")
#plt.ylabel("Bits set")
#plt.title(f"Bits set vs voltage")
#plt.show()

for vi in range(0, len(mv), 1):
	plt.plot(data[:,0], data[:,vi+1], label=f"{cols[vi+1]}")
plt.legend()
plt.xlabel("Sector")
plt.ylabel("Bits Set")
plt.title(f"Bits Set by Sector")
plt.show()

#for vi in range(0, len(mv), 1):
#	plt.plot(data[:,0], data[:,8], label=f"{cols[8]}")
#plt.xlabel("Sector")
#plt.ylabel("Bits Set")
#plt.title(f"Bits Set by Sector, {cols[8]}")
#plt.show()

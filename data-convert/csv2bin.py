from sys import argv
import numpy as np
from pathlib import Path
from multiprocessing import Pool

def check_voltage_completeness(dpath: Path) -> list[int]:
    if not dpath.is_dir():
        raise ValueError(f"[Integrity] Expected directory but {str(dpath)} is not a directory.")

    # mv : list of 1024 bools. True indicates that that sector index has been seen for that voltage
    vlists = {}

    # The CSV data is formatted as
    #     data-vvvv-sa.csv    where vvvv = read voltage in millivolts and sa is the decimal sector address (multiple of 65536)

    for fpath in dpath.iterdir():
        if fpath.suffix == '.csv' or fpath.suffix == '.CSV':
            parts = fpath.stem.split('-')
            # validate name format
            if len(parts) == 3 and parts[0] == 'data' and \
               parts[1].isascii() and parts[1].isnumeric() and \
               parts[2].isascii() and parts[2].isnumeric():
                #print(f"Found data CSV: {str(fpath.name)}")
                mv = int(parts[1]) # read millivolts
                sa = int(parts[2]) # sector address
                si = int(sa/65536) # sector index (65536 words in a sector)
                if not mv in vlists:
                    print(f"[Integrity] Found new voltage {mv} mV")
                    vlists[mv] = [False]*1024 # 1024 sectors in the chip
                vlists[mv][si] = True # we found it
            else:
                print(f"[Integrity] Skipping non-data CSV {str(fpath)}")
        else:
            print(f"[Integrity] Skipping non-csv file {str(fpath)}")

    bad = [mv for mv in vlists if not all(vlists[mv])]
    print(f"[Integrity] Incomplete voltages: {bad}")

    # return complete voltages
    return [mv for mv in vlists if all(vlists[mv])]

def convert_voltage(dpath: Path, mv: int, outpath: Path) -> None:
    if outpath.exists():
        print(f"[Convert] Output path exists. Exiting: {str(outpath)}")
        return
    # only called on validated complete dpath/mv

    print(f"[Convert] Converting {mv:4d} mV")

    with open(outpath, 'wb') as out:
        # loop over sectors
        for si in range(1024):
            sa = si * 65536

            print(f"[Convert] {mv:4d}mV: Converting {si}/1024 ({sa:d}) mV")

            # first file
            fname = f"data-{mv:4d}-{sa:d}.csv"
            fpath = dpath / fname

            # load text data
            txtdata = np.loadtxt(fpath, dtype=np.dtype("<H"), converters={0:lambda s: int(s,2)})
            b = bytes(txtdata)

            out.write(b)

    print(f"[Convert] Finished {mv:4d} mV")

if len(argv) < 2:
    print(f"Usage: {argv[0]} dir")
    exit(0)

dirpath = Path(argv[1])
complete = check_voltage_completeness(dirpath)

if len(argv) > 2:
    if argv[2] == 'check':
        print(complete)
    else:
        mv = int(argv[2])
        if mv in complete:
            convert_voltage(dirpath, mv, dirpath/f"data-{mv:4d}.bin")
        else:
            print(f"[csv2bin] Data is incomplete for {mv:4d} mV. Exiting.")
            exit(1)
else:
    print(f"[csv2bin] Complete voltages at {str(dirpath)}: {complete}")
    with Pool(processes=8) as pool:
        for mv in complete:
            pool.apply_async(convert_voltage, (dirpath, mv, dirpath/f"data-{mv:4d}.bin",))
        pool.close()
        pool.join()

print("Done")

#convert_voltage(dirpath, 6400, dirpath/'data-6400.bin')

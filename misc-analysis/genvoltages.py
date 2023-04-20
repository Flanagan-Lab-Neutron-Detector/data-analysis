""" Assign a voltage to each bit in a NISoC device """

from sys import argv
from pathlib import Path
import numpy as np
from multiprocessing import Pool

def convert_mv(mem, mem_range, data_mv, mv):
    for i in mem_range:
        b = int(mem[i])
        for j in range(8):
            if (b & (1 << j)) != 0 and data_mv[8*i + j] == 0:
                data_mv[8*i + j] = mv
    #bs = ' '.join([f"{int(b):02X}" for b in mem[mem_range[0]:mem_range[8]]])
    #vs = ' '.join([f"{v:4d}" for v in data_mv[8*mem_range[0]:8*(mem_range[-1]+1)][0:8]])
    #print(f"[{mv:4d}mv] {mem_range} [{bs}] first byte: [{vs}] mv")
    return data_mv[8*mem_range[0]:8*(mem_range[-1]+1)]


if __name__ == '__main__':

    if len(argv) < 3:
        print(f"Usage: {argv[0]} dir outfile")
        exit(0)

    dirpath = Path(argv[1])
    if not dirpath.is_dir():
        print(f"Expected directory but {str(dirpath)} is not a directory.")
        exit(1)

    outpath = Path(argv[2])
    if outpath.is_dir():
        print(f"Expected file but {str(dirpath)} is a directory.")
        exit(2)
    if outpath.exists():
        print(f"{str(dirpath)} already exists.")
        exit(3)

    mvs = set()

    for fpath in dirpath.iterdir():
        # bin name format:
        #    data-vvvv.bin    where vvvv is the read voltage in millivolts
        if fpath.suffix == '.bin':
            parts = fpath.stem.split('-')
            # validate name format
            if len(parts) == 2 and parts[0] == 'data' and \
               parts[1].isascii() and parts[1].isnumeric():
                mv = int(parts[1])
                mvs.add(mv)
                print(f"Found {mv:4d} mV")

    if len(mvs) == 0:
        print("No data found")
        exit(1)

    print("Allocating mv array")
    nsectors = 5
    mem_bytes = nsectors*1024*64*2
    mem_bits = mem_bytes * 8
    data_mv = np.zeros(mem_bits, dtype=np.dtype('<H'))

    for mv in sorted(mvs):
        fpath = dirpath / f"data-{mv:4d}.bin"
        print(f"Loading {str(fpath)}")
        data = fpath.read_bytes()
        with Pool(processes=8) as pool:
            denom = 16
            chunk_size = int(mem_bytes/denom)
            res = [pool.apply_async(convert_mv, (data, range(chunk_size*i, chunk_size*(i+1)), data_mv, mv,)) for i in range(denom)]
            for i,r in enumerate(res):
                thegoods = r.get(timeout=10)
                #print(thegoods)
                data_mv[8*chunk_size*i:8*chunk_size*(i+1)] = thegoods[:]
            pool.close()
            pool.join()
        
    print(f"Writing to {str(outpath)}")
    with open(outpath, 'wb') as outfile:
        outfile.write(bytes(data_mv))

    print("Done")

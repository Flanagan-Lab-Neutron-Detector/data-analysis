# nisoc_readers.py

"""
Routines to read nisoc raw data files (bin and csv) by word, byte, and bit.
"""

def read_file(path):
    """Read a nisoc file by word (line). Yields the bits in each word as a list."""
    suffix = path.suffix.lower()
    if suffix == '.bin':
        return read_bin(path)
    elif suffix == '.csv':
        return read_csv(path)
    else:
        raise ValueError(f"Unknown file type: {suffix}")

def read_bin(path):
    """Read a nisoc binary file by word. Yields the bits in each word as a list."""
    with open(path, 'rb') as f:
        word = f.read(2)
        while word:
            yield [int(x) for x in bin(int.from_bytes(word, byteorder='little'))[2:].zfill(16)]
            word = f.read(2)

def read_bin_by_byte(path):
    """Read a nisoc binary file by byte. Yields the bits in each byte as a list."""
    with open(path, 'rb') as f:
        byte = f.read(1)
        while byte:
            yield [int(x) for x in bin(int.from_bytes(byte, byteorder='little'))[2:].zfill(8)]
            byte = f.read(1)

def read_bin_by_bit(path):
    """Read a nisoc binary file by bit. Yields each bit as an integer."""
    for byte in read_bin_by_byte(path):
        for bit in byte:
            yield bit

def read_csv(path):
    """Read a nisoc "csv" file by word (line). Yields the bits in each word as a list."""
    with open(path, 'r') as f:
        for line in f:
            yield [int(x) for x in line.strip()]

def read_csv_by_byte(path, big_endian=True):
    """Read a nisoc "csv" file by byte. Yields the bits in each byte as a list."""
    with open(path, 'r') as f:
        for line in f:
            line = line.strip()
            msb = [int(x) for x in line[0:8]]
            lsb = [int(x) for x in line[8:16]]
            if big_endian:
                yield msb
                yield lsb
            else:
                yield lsb
                yield msb

def read_csv_by_bit(path, big_endian=True):
    """Read a nisoc "csv" file by bit. Yields each bit as an integer."""
    for byte in read_csv_by_byte(path, big_endian):
        for bit in byte:
            yield bit

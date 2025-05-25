def pack_numbers(a, b, c, d):
    result = (d << (10 + 6 + 7)) | (c << (10 + 6)) | (b << 10) | a
    return result

def unpack_numbers(packed):
    a = packed & ((1 << 10) - 1)
    b = (packed >> 10) & ((1 << 6) - 1)
    c = (packed >> (10 + 6)) & ((1 << 7) - 1)
    d = (packed >> (10 + 6 + 7)) & ((1 << 9) - 1)
    return a, b, c, d

# a, b, c, d = 212, 4, 31, 14
a, b, c, d = 479, 9, 1, 32

packed = pack_numbers(a, b, c, d)
print(f"Packed: {packed:032b}, {packed}")  # Print as 32-bit binary
print("Unpacked:", unpack_numbers(packed))
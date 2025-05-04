packed_array = []

# for segment in range(64):
#     # Create the base pattern: 5 repetitions of 6-bit segment number
#     pattern = 0
#     for _ in range(5):
#         pattern = (pattern << 6) | segment
#     pattern <<= 2  # Add two trailing 0 bits

#     # Append 960 times for each segment
#     packed_array.extend([pattern] * 960)
#-------------------------------------------------------------------------------------------
# k = 61440
# for i in range(k):
#     if i > 0.45*k and i < 0.55*k:
#         packed_array.append(204522252)
#     else:
#         packed_array.append(818089008)
#-------------------------------------------------------------------------------------------
# for y in range(480):
#     for x in range(128):
#         if y%3 == 0:
#             packed_array.append(204522252)
#         if y%3 == 1:
#             packed_array.append(818089008)
#         if y%3 == 2:
#             packed_array.append(3272356032)
#-------------------------------------------------------------------------------------------
for y in range(480):
    for x in range(128):
        if x < 0.5*128:
            packed_array.append(204522252)
        else:
            packed_array.append(818089008)

def save_as_c_array(packed_array, filename):
    with open(filename, 'w') as file:
        file.write("#include <stdint.h>\n\n")
        file.write("uint32_t pixel_data[] = {\n")

        for i, value in enumerate(packed_array):
            if i % 8 == 0:  # Format nicely with new lines every 8 values
                file.write("    ")
            file.write(f"0x{value:08X}, ")

            if (i + 1) % 8 == 0 or i == len(packed_array) - 1:
                file.write("\n")

        file.write("};\n")

# Example usage
output_file = "pixel_data.h"
save_as_c_array(packed_array, output_file)

print(f"C-style array saved to {output_file}, count = {len(packed_array)}")
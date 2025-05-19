from PIL import Image
import numpy as np

def reduce_color_to_2bit(value):
    """Reduce 8-bit color to 2-bit (0-255 → 0-3)."""
    return value >> 6  # 8-bit to 2-bit by right-shifting 6 bits

def pack_five_pixels(pixels):
    """Pack 5 reduced-color pixels (2 bits per RGB channel = 6 bits per pixel) into a 32-bit int."""
    packed = 0
    for i, (r, g, b) in enumerate(pixels):
        r2 = reduce_color_to_2bit(r)
        g2 = reduce_color_to_2bit(g)
        b2 = reduce_color_to_2bit(b)
        six_bits = (r2 << 4) | (g2 << 2) | b2
        shift = (4 - i) * 6 + 2  # leave 2 LSBs as 0
        packed |= (six_bits << shift)
    return packed

def image_to_packed_array(image_path):
    # Load and resize the image
    img = Image.open(image_path).convert('RGB')
    img = img.resize((640, 480))
    pixels = list(img.getdata())
    
    # Pack every 5 pixels into one 32-bit integer
    packed_array = []
    for i in range(0, len(pixels), 5):
        group = pixels[i:i+5]
        if len(group) < 5:
            # Pad with black pixels if not enough
            group += [(0, 0, 0)] * (5 - len(group))
        packed = pack_five_pixels(group)
        packed_array.append(packed)
    
    for i in range(len(packed_array)):
        if i % 640 == 0:
            packed_array[i] = 0;
    
    return packed_array


packed_data = image_to_packed_array("pixs/cat.JPG")
print(packed_data[:10])  # Print first 10 packed integers


print(f"count: {len(packed_data)}")

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
save_as_c_array(packed_data, output_file)

print(f"C-style array saved to {output_file}")


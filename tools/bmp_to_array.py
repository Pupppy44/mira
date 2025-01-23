#!/usr/bin/env python3

import sys
from PIL import Image

# Hardcoded VGA Mode 13h 256-color palette (RGB tuples, 0-63 range scaled to 0-255 for display)
VGA_PALETTE = [
    (i, j, k) for i in range(0, 256, 64)
    for j in range(0, 256, 64)
    for k in range(0, 256, 64)
][:256]  # Ensure only 256 entries

def scale_to_vga_palette(color):
    """Scale a 24-bit RGB color to the closest match in the VGA 256-color palette."""
    r, g, b = color
    scaled_color = (r // 4, g // 4, b // 4)  # Scale 0-255 to VGA's 0-63
    closest = min(range(len(VGA_PALETTE)), key=lambda i: (
        (VGA_PALETTE[i][0] - scaled_color[0]) ** 2 +
        (VGA_PALETTE[i][1] - scaled_color[1]) ** 2 +
        (VGA_PALETTE[i][2] - scaled_color[2]) ** 2
    ))
    return closest

def convert_to_8bit_c_array(input_path, output_c_file):
    """
    Converts a BMP image into an 8-bit indexed uint8_t C array using a hardcoded VGA palette.
    """
    # Open the original BMP
    img = Image.open(input_path).convert('RGB')
    width, height = img.size

    # Output the VGA palette as uint8_t array
    with open(output_c_file, "w") as f:
        # Write the hardcoded VGA palette as a uint8_t array
        f.write("const uint8_t vga_palette[256][3] = {\n")
        for r, g, b in VGA_PALETTE:
            f.write(f"    {{ {r}, {g}, {b} }},\n")
        f.write("};\n\n")

        # Write the indexed image data as uint8_t array
        f.write(f"const uint8_t image_data[{width} * {height}] = {{\n")

        # Map pixels to VGA palette indices
        pixels = img.load()
        for y in range(height):
            for x in range(width):
                color = pixels[x, y]
                index = scale_to_vga_palette(color)
                f.write(f"{index}, ")
            f.write("\n")
        f.write("};\n")

def main():
    if len(sys.argv) < 3:
        print("Usage: python convert_to_8bit.py <input_bmp> <output_c_file>")
        sys.exit(1)

    input_bmp = sys.argv[1]
    output_c_file = sys.argv[2]
    convert_to_8bit_c_array(input_bmp, output_c_file)
    print(f"Converted {input_bmp} to an 8-bit indexed uint8_t array and saved as {output_c_file}.")

if __name__ == "__main__":
    main()

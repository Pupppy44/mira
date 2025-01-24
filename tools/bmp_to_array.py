#!/usr/bin/env python3
"""
bmo_to_indexed.py

Converts a fictional .bmo file (with width, height, and raw 24-bit pixels)
into a C-style static uint8_t array of palette indices. The palette must match
the kernel's mk_gfx_colors for accurate color matching.
"""

import sys
import math

# Same 256-color palette used in your kernel, as an array of (R,G,B) from 0..255
mk_gfx_colors = [
    # The first 16 EGA entries:
    (0,   0,   0),   (0,   0,   170), (0,   170, 0),   (0,   170, 170),
    (170, 0,   0),   (170, 0,   170), (170, 85,  0),   (170, 170, 170),
    (85,  85,  85),  (85,  85,  255), (85,  255, 85),  (85,  255, 255),
    (255, 85,  85),  (255, 85,  255), (255, 255, 85),  (255, 255, 255),
]

# Fill out the next entries: 6x6x6 color cube from index=16..16+216-1=231, then grayscale
# For an exact match to your kernel array, replicate that logic here:
for r in range(6):    # 0..5
    for g in range(6):
        for b in range(6):
            mk_gfx_colors.append((r * 51, g * 51, b * 51))
# Similarly, 24 grayscale steps from 232..255
for i in range(24):
    val = i * 11
    mk_gfx_colors.append((val, val, val))


def read_bmo_header(f):
    """
    Mock function to read a .bmo header which might contain width, height.
    Adjust to match your real .bmo format.
    """
    # For example, first 4 bytes: width, next 4 bytes: height, both little-endian
    width_bytes = f.read(4)
    height_bytes = f.read(4)
    if len(width_bytes) < 4 or len(height_bytes) < 4:
        raise ValueError("File too small or truncated for .bmo header.")

    width = int.from_bytes(width_bytes, byteorder='little', signed=False)
    height = int.from_bytes(height_bytes, byteorder='little', signed=False)
    return width, height


def read_bmo_pixels(f, width, height):
    """
    Reads width*height 24-bit pixels (R,G,B) from the file.
    Returns a list of (R,G,B) tuples.
    """
    pixels = []
    num_pixels = width * height
    for _ in range(num_pixels):
        rgb = f.read(3)
        if len(rgb) < 3:
            raise ValueError("Unexpected end of file in pixel data.")
        R = rgb[0]
        G = rgb[1]
        B = rgb[2]
        pixels.append((R, G, B))
    return pixels


def find_nearest_palette_index(r, g, b):
    """
    Returns the palette index in mk_gfx_colors that is closest to (r,g,b).
    Simple Euclidean distance in RGB space.
    """
    min_dist = float('inf')
    best_index = 0
    for idx, (pr, pg, pb) in enumerate(mk_gfx_colors):
        # Euclidean distance
        dist = (r - pr)**2 + (g - pg)**2 + (b - pb)**2
        if dist < min_dist:
            min_dist = dist
            best_index = idx
    return best_index


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <input.bmo>")
        sys.exit(1)

    bmo_file = sys.argv[1]
    with open(bmo_file, "rb") as f:
        width, height = read_bmo_header(f)
        pixels = read_bmo_pixels(f, width, height)

    # Convert each pixel to nearest palette index
    indexed_data = []
    for (r, g, b) in pixels:
        idx = find_nearest_palette_index(r, g, b)
        indexed_data.append(idx)

    # Output a C array of size width*height with the name you prefer
    # e.g. "unsigned char my_bmo_data[] = { ... };"
    print(f"// Width={width}, Height={height}\n")
    print(f"static const uint8_t bmo_image[{width * height}] = {{")
    for i, color_idx in enumerate(indexed_data):
        end_char = "," if i < (len(indexed_data) - 1) else ""
        # Print a new line every 16 entries for readability
        if i % 16 == 0:
            print("    ", end="")
        print(f"{color_idx:3d}{end_char}", end=" ")
        if (i + 1) % 16 == 0:
            print("")  # new line
    print("\n};")
    print("// Use bmo_image with mk_gfx_draw_bitmap(bmo_image, width, height, x, y).")


if __name__ == "__main__":
    main()
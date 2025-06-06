from PIL import Image

def png_to_vga13h_array(png_filepath, output_c_filepath):
    # 1) Open and force to RGBA (so we can detect alpha). Then resize to exactly 320x200 if needed:
    img = Image.open(png_filepath).convert("RGBA")
    if img.size != (320, 200):
        img = img.resize((320, 200), Image.NEAREST)

    # 2) If pixel is mostly transparent, paint it black before quantizing:
    pixels = img.load()
    for y in range(200):
        for x in range(320):
            r, g, b, a = pixels[x, y]
            if a < 200:
                # Force that pixel to pure black (0,0,0) with alpha = 255
                pixels[x, y] = (0, 0, 0, 255)

    # 3) Convert RGBA→RGB (dropping alpha), then quantize to 256 colors:
    rgb = img.convert("RGB")
    paletted = rgb.quantize(colors=256, method=0)  # method=0 is okay for RGB images

    # 4) Extract raw byte indexes:
    pixel_indexes = list(paletted.getdata())  # length = 320*200

    # 5) Write out the C file:
    with open(output_c_filepath, "w") as f:
        f.write("// mode 13h indexed image (320x200)\n")
        f.write("unsigned char image_data[320*200] = {\n")
        for row in range(200):
            row_slice = pixel_indexes[row*320 : (row+1)*320]
            line = ", ".join(str(byte) for byte in row_slice)
            if row < 199:
                f.write("    " + line + ",\n")
            else:
                f.write("    " + line + "\n")
        f.write("};\n")

    print(f"Wrote {output_c_filepath} (320x200, 8-bit VGA indices).")

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python3 png_to_vga13h_array.py <input.png> <output.c>")
        sys.exit(1)

    inp, outp = sys.argv[1], sys.argv[2]
    png_to_vga13h_array(inp, outp)

from PIL import Image

def png_to_vga_indexed_c_array(png_filepath, output_filepath):
    """
    Converts a 128x128 PNG image to an 8-bit VGA 13h indexed C char array.
    Transparent pixels are converted to pure black (index 0).

    Args:
        png_filepath: Path to the input PNG file.
        output_filepath: Path to save the output C file.
    """

    try:
        img = Image.open(png_filepath)
        if img.size != (128, 128):
            raise ValueError("Input PNG must be 128x128 pixels.")

        # Convert to RGBA to handle transparency
        img = img.convert("RGBA")
        pixels = img.load()

        # Initialize the C array data
        c_array_data = []

        for y in range(128):
            for x in range(128):
                r, g, b, a = pixels[x, y]

                if a < 200:  # Consider pixels with low alpha as transparent
                    c_array_data.append(0)  # Black index (assuming black is at index 0 in VGA palette)
                else:
                    # Simple grayscale conversion for indexing (can be improved with proper palette mapping)
                    # This just assigns indices based on brightness. A real VGA palette
                    # would require a more sophisticated color matching algorithm.
                    gray = int(0.299 * r + 0.587 * g + 0.114 * b)
                    index = min(255, gray) # Clamp to 0-255
                    c_array_data.append(index)

        # Format the C array string
        c_array_string = ",\n".join(map(str, c_array_data))
        c_code = f"""// VGA 13h indexed image data (128x128)
unsigned char image_data[] = {{\n{c_array_string}\n}};
"""

        # Save to the output C file
        with open(output_filepath, "w") as f:
            f.write(c_code)

        print(f"Successfully converted '{png_filepath}' to '{output_filepath}'")

    except FileNotFoundError:
        print(f"Error: PNG file not found at '{png_filepath}'")
    except ValueError as e:
        print(f"Error: {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

if __name__ == "__main__":
    input_png = "mira.png"  # Replace with your input PNG file
    output_c = "image_data.c"  # Replace with your desired output C file name
    png_to_vga_indexed_c_array(input_png, output_c)
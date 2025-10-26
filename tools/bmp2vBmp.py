from PIL import Image
import sys
import os

def convert_bmp_to_8_indexed(input_path, output_path, show_palette=False):
    """Convert a BMP to 8-color indexed BMP.

    If output_path == input_path, overwrite safely using a temp file to avoid
    Windows file locking issues when reading and writing the same path.
    """
    # Load image (keep a handle to close if overwriting)
    img = Image.open(input_path)
    img.load()  # force load so we can close safely

    # Convert to 8-color palette image using an adaptive palette
    img_quantized = img.convert('P', palette=Image.ADAPTIVE, colors=8)

    # Overwrite handling: Windows can't write to a file that's still open for read
    same_path = os.path.abspath(input_path) == os.path.abspath(output_path)
    if same_path:
        # Close original before writing
        img.close()
        temp_path = output_path + ".tmp"
        img_quantized.save(temp_path, format='BMP')
        os.replace(temp_path, output_path)
    else:
        img_quantized.save(output_path, format='BMP')

    print(f"Saved 8-color indexed BMP to: {output_path}")

    if show_palette:
        # Get the actual palette (returns up to 768 values, 256 * RGB)
        palette = img_quantized.getpalette()[:24]  # Only first 8*3 values (8 colors)
        print("Palette (RGB):")
        for i in range(8):
            r, g, b = palette[i*3:i*3+3]
            print(f"Index {i}: ({r}, {g}, {b})")

        # Show indexed values for each pixel (as a 2D list)
        pixels = list(img_quantized.getdata())
        width, height = img_quantized.size
        pixel_2d = [pixels[i * width:(i + 1) * width] for i in range(height)]

        print("\nIndexed pixel data:")
        for row in pixel_2d:
            print(row)

if __name__ == "__main__":
    show = "--show" in sys.argv
    # Collect non-flag arguments (input and optional output)
    args = [a for a in sys.argv[1:] if a != "--show"]

    if len(args) < 1:
        print("Usage: python bmp_to_8indexed.py <input.bmp> [output.bmp] [--show]")
        sys.exit(1)

    input_path = args[0]
    output_path = args[1] if len(args) >= 2 else input_path  # Default to overwrite input if no output given

    convert_bmp_to_8_indexed(input_path, output_path, show_palette=show)

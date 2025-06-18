# assets_to_mfs.py
# This script packs all files in an assets.bin
# for the Mira Kernel to load at runtime.

import sys
import os
import struct

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ASSETS_DIR = os.path.join(ROOT, "assets")
OUT_FILE = os.path.join(ROOT, "build", "assets.bin")

MAGIC = 0x6D667330  # "mfs0"

def get_asset_files():
    files = []
    for root, dirs, names in os.walk(ASSETS_DIR):
        for name in names:
            path = os.path.join(root, name)
            if os.path.isfile(path):
                files.append(path)
    files.sort()
    return files

def main():
    files = get_asset_files()
    if not files:
        print("[mfs] No files found in assets directory.")
        sys.exit(0)

    header_size = 8
    table = bytearray()
    payload = bytearray()

    for fpath in files:
        data = open(fpath, "rb").read()
        rel = os.path.relpath(fpath, ASSETS_DIR)
        name_bytes = rel.replace(os.sep, "/").encode("utf-8")
        entry = struct.pack("<B", len(name_bytes))
        entry += name_bytes
        entry += struct.pack(
            "<II", # <II = LE unsigned int
            header_size + len(table) + len(payload),
            len(data)
        )
        table += entry
        payload += data

    count = len(files)
    blob = struct.pack("<II", MAGIC, count) + table + payload

    os.makedirs(os.path.dirname(OUT_FILE), exist_ok=True)
    open(OUT_FILE, "wb").write(blob)

    print(f"[mfs] Packed {str(count)} files(s) -> {OUT_FILE} ({len(blob)} bytes)")

if __name__ == "__main__":
    main()

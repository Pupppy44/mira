#!/bin/bash

echo "Debugging with Qemu..."

/mnt/d/x/qemu/qemu-system-x86_64.exe -vga std -drive file=./build/mira.img,format=raw
#!/bin/bash

echo "Debugging with Qemu..."

/mnt/d/x/qemu/qemu-system-x86_64.exe -drive file=./build/mira.img,format=raw
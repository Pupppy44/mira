#!/bin/bash

echo "Debugging with Qemu..."

# Drive: Mira image file (mira.img)
# Audio: DirectSound for Windows to play audio. 
/mnt/d/x/qemu/qemu-system-x86_64.exe -drive file=./build/mira.img,format=raw -audiodev dsound,id=speaker -machine pcspk-audiodev=speaker
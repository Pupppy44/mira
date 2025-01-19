#!/bin/bash

echo "Debugging with Qemu..."

# Drive: Mira image file (mira.img)
# Audio: DirectSound for Windows to play audio. 
#/mnt/d/x/qemu/qemu-system-x86_64.exe -drive file=./build/mira.img,format=raw -audiodev dsound,id=speaker -machine pcspk-audiodev=speaker

# D:\X\qemu_old\qemu\qemu-system-x86_64.exe -drive file=./build/mira.img,format=raw -audiodev dsound,id=speaker -machine pcspk-audiodev=speaker

/mnt/d/x/qemu_old2/qemu/qemu-system-x86_64.exe -soundhw sb16 -drive file=./build/mira.img,format=raw
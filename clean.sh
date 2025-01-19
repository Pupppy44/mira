#!/bin/bash

find . -type f -name '*.o' -delete

find . -type f -name '*.elf' -delete

find . -type f -name '*.bin' -delete

(cd ./bootloader ; rm -f boot)

(cd ./kernel ; rm -f kernel)

echo "Clean finished successfully"
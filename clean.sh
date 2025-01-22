#!/bin/bash

find . -type f -name '*.o' -delete

find . -type f -name '*.elf' -delete

find . -type f -name '*.bin' -delete

(cd ./boot ; rm -f boot)

(cd ./kernel ; rm -f kernel)

(cd ./shell ; rm -f shell)

echo "Clean finished successfully"
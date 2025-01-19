#!/bin/bash

rm -f build/mira.img

(make -C bootloader)
boot_result=$?

(make -C kernel)
make_result=$?

if (test -f bootloader/boot.bin) && (test -f kernel/kernel.bin)
then
    echo "Build successful"
else
    echo "Build failed"
fi

if [ "$boot_result" = "0" ] && [ "$make_result" = "0" ]
then
    kernel_size=$(wc -c < kernel/kernel.bin)
    kernel_sectors=$(( ($kernel_size + 511) / 512 ))
    printf %02x $kernel_sectors | xxd -r -p | dd of=bootloader/boot.bin bs=1 seek=2 count=1 conv=notrunc

    cp bootloader/boot.bin ./build/mira.img
    cat kernel/kernel.bin >> build/mira.img

    echo "Build finished successfully"

    sh clean.sh
    
    sh debug.sh
else
    result=`expr $boot_result + $make_result`
    echo "Build failed with error code $result. See output for more info."

    sh clean.sh
fi
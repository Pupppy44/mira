#!/bin/bash

rm -f build/mira.img

(make -C boot)
boot_result=$?

# IMPORTANT: Build shell FIRST for the kernel to access its entry function
(make -C shell)
shell_result=$?

(make -C kernel)
kernel_result=$?

if (test -f boot/boot.bin) && (test -f kernel/kernel.bin) && (test -f shell/shell.bin)
then
    echo "Build successful"
else
    echo "Build failed"
fi

if [ "$boot_result" = "0" ] && [ "$kernel_result" = "0" ] && [ "$shell_result" = "0" ]
then
    kernel_size=$(wc -c < kernel/kernel.bin)
    kernel_sectors=$(( ($kernel_size + 511) / 512 ))
    printf %02x $kernel_sectors | xxd -r -p | dd of=boot/boot.bin bs=1 seek=2 count=1 conv=notrunc

    cp boot/boot.bin ./build/mira.img
    cat kernel/kernel.bin >> build/mira.img

    echo "Build finished successfully"

    sh clean.sh
    
    sh debug.sh
else
    result=`expr $boot_result + $make_result`
    echo "Build failed with error code $result. See output for more info."

    sh clean.sh
fi
# Mira - Experimental 64-Bit Operating System Kernel

## About

Mira is a personal exploration into building a 64-bit operating system kernel from the ground up, written primarily in C and Assembly. My focus has been on understanding the foundational aspects of OS design, including boot processes, memory management, and basic task scheduling. While Mira is an ongoing learning project and not intended for daily use, it represents a deep dive into low-level systems programming.

## Current Features

*   **Multi-Stage Bootloader:** Boots from 16-bit Real Mode, through 32-bit Protected Mode, and into 64-bit Long Mode.
*   **Global Descriptor Table (GDT) Setup:** Implemented for both 32-bit and 64-bit modes.
*   **Interrupt Descriptor Table (IDT) Initialization:** Basic IDT setup with default handlers and specific entries for PIT and a simple syscall.
*   **Paging Enabled:** Basic page table setup to identity map the lower physical memory.
*   **Programmable Interval Timer (PIT) Integration:** PIT initialized for timed interrupts.
*   **Foundational Kernel Services:**
    *   Basic bump allocator for memory management.
    *   Simple round-robin task scheduler concept.
    *   Minimal syscall interface (direct VGA character/pixel output).
*   **Build System:** Utilizes Makefiles with Clang and NASM for building the bootloader and kernel.

## What I Learned

Working on Mira has been a challenging but rewarding journey into:
*   x86 architecture and its different operating modes.
*   The complexities of CPU initialization and memory segmentation/paging.
*   Interrupt handling and timer mechanisms.
*   The fundamentals of kernel design and basic process management.
*   Low-level debugging techniques.

## Building

To build Mira, you will need the following tools:
*   `clang`
*   `nasm`
*   `gdb` (for debugging)
*   `ld.lld`

There is a `build.sh` script in the root directory that will build Mira for you. To run the script, simply execute the following command:
```bash
./build.sh
```

## Running

To run Mira, you will need to install QEMU. Once you have QEMU installed, you can run Mira by executing the following command:
```bash
qemu-system-x86_64 -cdrom build/mira.img
```

## Future Exploration

While current development is focused on polishing the core kernel, future areas I'd like to explore include:
*   More robust memory management.
*   A simple filesystem.
*   Basic device drivers (keyboard, mouse).
*   A minimal graphical user interface.

## License

Mira is licensed under the MIT License. See the LICENSE file for more information.
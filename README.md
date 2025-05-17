<div align="center">
    <img src="https://i.ibb.co/prR8ZBhY/mira.png" alt="Mira Logo" />

</div>

# Mira - Experimental 64-Bit Operating System Kernel

## About
Mira is a personal exploration into building a 64-bit operating system kernel from the ground up, written primarily in C and Assembly. My focus has been on understanding the foundational aspects of OS design, including boot processes, memory management, and basic task scheduling. While Mira is still in development and not intended for daily use, it represents a deep dive into creating operating systems from scratch.

## Current Features

* **Multi-Stage Bootloader:** Boots from 16-bit Real Mode, through 32-bit Protected Mode, and into 64-bit Long Mode.
* **Paging Enabled:** Basic page table setup to identity map the lower physical memory.
* **Global Descriptor Table (GDT) Setup:** Implemented for both 32-bit and 64-bit modes.
* **Interrupt Descriptor Table (IDT) Initialization:** Basic IDT setup with default handlers and specific entries for PIT and a simple syscall.
* **Programmable Interval Timer (PIT) Integration:** PIT initialized for timed interrupts and basic scheduling.
* **Core Kernel Services:**
    * Basic bump allocator for memory management.
    * Basic keyboard input handling.
    * Simple round-robin task scheduler.
    * Minimal syscall interface (direct VGA character/pixel output).

## What I Learned

Working on Mira has been a challenging but rewarding journey into:
* x86 architecture and its different operating modes.
* The complexities of CPU initialization and memory segmentation/paging.
* Interrupt handling and timer mechanisms.
* The fundamentals of kernel design and basic process management.
* Low-level device interaction (keyboard input, VGA output).

## Building

To build Mira, you will need the following tools:
* `clang`
* `nasm`
* `gdb` (for debugging)
* `ld.lld`

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
* More advanced memory and process management.
* A simple filesystem (FAT32 or a custom one).
* Basic device drivers (PS/2 mouse, Sound Blaster 16 audio, etc.).
* A minimal graphical user interface with window management.
* User-mode applications (notepad, simple games).
* Networking stack (UDP/TCP/IP).

## License

Mira is licensed under the MIT License. See the LICENSE file for more information.
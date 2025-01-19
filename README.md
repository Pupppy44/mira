# Mira - 64-Bit Graphical Operating System

## About

Mira is a 64-bit graphical operating system written in C++ and Assembly. With a focus on speed and simplicity, Mira is designed to be a lightweight and efficient operating system. Mira is currently in development and is not yet ready for daily use.

## Features

- 64-bit kernel
- Graphical desktop
- User programs

## Building

To build Mira, you will need the following tools:
- clang
- nasm
- gdb
- ld.lld

There is a `build.sh` script in the root directory that will build Mira for you. To run the script, simply execute the following command:

```bash
./build.sh
```

## Running

To run Mira, you will need to install QEMU. Once you have QEMU installed, you can run Mira by executing the following command:

```bash
qemu-system-x86_64 -cdrom build/mira.img
```

## License

Mira is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
```
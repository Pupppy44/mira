/* The ENTRY point should match your actual assembly entry symbol. */
ENTRY(_start)

/* Output format for x86_64 ELF. */
OUTPUT_FORMAT("elf64-x86-64")

SECTIONS
{
    /* Start at 0x8200 */
    . = 0x8200;

    /* .text section: code. Align on page boundary (0x1000). */
    .text ALIGN(0x1000) : {
        *(.text)
        *(.text.*)
    }

    /* .rodata section: read‐only data, also aligned. */
    .rodata ALIGN(0x1000) : {
        *(.rodata)
        *(.rodata.*)
    }

    /* .data section: initialized data, aligned as well. */
    .data ALIGN(0x1000) : {
        *(.data)
        *(.data.*)
    }

    /* .relro section: read-only after relocation. */
    .relro ALIGN(0x1000) : {
        *(.got)
        *(.got.*)
        *(.got.plt)
        *(.data.rel.ro*)
        *(.data.rel.ro.*)
        PROVIDE_HIDDEN(__relro_end = .);
    }

    /* .bss section: uninitialized data. */
    .bss ALIGN(0x1000) : {
        /* Common and BSS symbols. */
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
}
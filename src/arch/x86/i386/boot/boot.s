/* src/boot.s - Multiboot2 header + entry point with framebuffer request */

.section .multiboot2, "a"
.align 8
multiboot_header:
    .long 0xe85250d6          # magic
    .long 0                   # architecture (i386)
    .long multiboot_header_end - multiboot_header  # header length
    .long -(0xe85250d6 + 0 + (multiboot_header_end - multiboot_header)) # checksum

    # --- Framebuffer header tag (type = 5) ---
    # Ask GRUB for any resolution, 32 bpp. Optional (flags = 0).
    .align 8
    .short 5                  # u16 type  = 5 (framebuffer request)
    .short 0                  # u16 flags = 0 (optional)
    .long 20                  # u32 size  = 20 bytes
    .long 0                   # width  = 0 (any)
    .long 0                   # height = 0 (any)
    .long 32                  # depth  = 32 bpp

    # --- End tag (type = 0, size = 8) ---
    .align 8
    .short 0                  # type
    .short 0                  # flags
    .long 8                   # size
multiboot_header_end:

.section .text, "ax"
.code32
.global _start
.extern kmain

_start:
    # Multiboot2 spec:
    #   EAX = magic (0x36d76289)
    #   EBX = pointer to multiboot2 info struct

    push %ebx       # pass mb2 info pointer as first arg to kmain
    call kmain
    add $4, %esp

hang:
    cli
    hlt
    jmp hang

.section .note.GNU-stack,"",@progbits


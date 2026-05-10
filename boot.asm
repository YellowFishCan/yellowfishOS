org 0x7c00
bits 16

start:
    cli

    mov ax, 0x1000
    mov es, ax
    mov bx, 0

    mov ah, 0x02
    mov al, 0x04

    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, 0x80
    int 0x13

    lgdt [gdtr]
    mov eax, cr0
    or eax, 1
    mov cr0, eax


    jmp dword 0x08:protected_mode

gdtr:
    dw gdt_end - gdt - 1
    dd gdt

gdt:
    dd 0, 0

    dw 0xFFFF
    dw 0
    db 0
    db 10011010b
    db 11001111b
    db 0

    dw 0xFFFF
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0
gdt_end:

bits 32
protected_mode:

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9C000
    mov ebp, esp

    mov esi, protected_msg
    mov edi, 0xB8000

    mov edi, 0xB8000
    mov ecx, 80 * 25
    mov al, ' '
    mov ah, 0x0F
    rep stosw

    call 0x08:0x10000
    jmp $

.print_loop:
    lodsb
    or al, al
    jz .done
    mov [edi], al
    mov byte [edi+1], 0x0F
    add edi, 2
    jmp .print_loop
      
.done:
    jmp $

protected_msg db "yellowfishOS a0.0.0 . in x86 . is done!", 0

times 510-($-$$) db 0
dw 0xAA55
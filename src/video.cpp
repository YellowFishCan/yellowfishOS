#include "video.h"

#define uint16_t unsigned short
#define uint32_t unsigned int
#define uint8_t unsigned char

Cursor cursor = { 0,0 };

void useCursor() {
    unsigned short position = cursor.row * 80 + cursor.col;
    outb(0x3D4, 0x0F);
    outb(0x3d5, (position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3d5, (position >> 8) & 0xFF);
}

void setCursor(int row, int col) {
    cursor.row = row;
    cursor.col = col;
    useCursor();
}

int getCol() {
    return cursor.col;
}

int getRow() {
    return cursor.row;
}

void scroll() {
    volatile unsigned short* video = (unsigned short*)0xB8000;

    for (int row = 0; row < 24; row++) {
        for (int col = 0; col < 80; col++) {
            video[row * 80 + col] = video[(row + 1) * 80 + col];
        }
    }

    for (int col = 0; col < 80; col++) {
        video[24 * 80 + col] = (0x0F << 8) | ' ';
    }

    cursor.row = 24;
    cursor.col = 0;
    useCursor();
}

void clear_screen() {
    asm volatile(
        "movl $0xB8000, %%edi\n\t"
        "movl $2000, %%ecx\n\t"
        "movw $0x0F20, %%ax\n\t"
        "rep stosw\n\t"
        :
    :
        : "edi", "ecx", "eax", "memory"
        );
    setCursor(0, 0);
}

void print_char(char c) {
    volatile unsigned short* video = (unsigned short*)0xB8000;

    if (c == '\n') {
        cursor.col = 0;
        cursor.row++;
    }
    else {
        int offset = cursor.row * 80 + cursor.col;
        video[offset] = (0x0F << 8) | c;
        cursor.col++;
        if (cursor.col >= 80) {
            cursor.col = 0;
            cursor.row++;
        }
    }

    if (cursor.row >= 25) {
        scroll();
        cursor.row = 24;
        cursor.col = 0;
    }
    useCursor();
}

void print(const char* msg) {
    volatile char* video = (char*)0xB8000;
    int i = 0;
    while (msg[i]) {
        print_char(msg[i++]);
    }
}

// keyboard input!

struct IDTEntry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t base_high;
} __attribute__((packed));

IDTEntry idt[256];

char keyboard_map[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
    '\b', 0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    0, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void int80_handler() {
    print("int 0x80 triggered!\n");
}

void set_idt_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t type_attr) {
    idt[num].base_low = handler & 0xFFFF;
    idt[num].base_high = (handler >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = type_attr;
}

void keyboard_handler_wrapper() {

}

void dummy_handler() {
    while (1) {
        asm("hlt");
    }
}

#define BUFFER_SIZE 256
char input_buffer[BUFFER_SIZE];
int buffer_head = 0;
int buffer_tail = 0;

void keyboard_handler() {
    asm volatile (
        "pusha\n"
        "call keyboard_handler_c\n"
        "popa\n"
        "iret\n"
        );
}

char getchar() {
    while (buffer_head == buffer_tail) {
        asm("hlt");
    }
    char c = input_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) & (BUFFER_SIZE - 1);
    return c;
}

void idt_init() {
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0xFD);
    outb(0xA1, 0xFF);
    for (int i = 0; i < 256; i++) {
        set_idt_gate(i, (uint32_t)dummy_handler, 0x08, 0x8E);
    }
    set_idt_gate(33, (uint32_t)keyboard_handler, 0x08, 0x8E);

    struct {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed)) idt_ptr = { sizeof(idt) - 1, (uint32_t)idt };
    asm volatile("lidt %0" : : "m"(idt_ptr));
}

void keyboard_handler_c() {
    unsigned char scancode = inb(0x60);
    print_char('1');
    if (scancode == 0x1C) {
        print_char('2');
        char c = '\n';
        int next = (buffer_head + 1) & (BUFFER_SIZE - 1);
        
        if (next != buffer_tail) {
            input_buffer[buffer_head] = c;
            buffer_head = next;
        }
        
    }
    else if (!(scancode & 0x80)) {
        print_char('3');
        char c = keyboard_map[scancode];
        if (c) {
            int next = (buffer_head + 1) & (BUFFER_SIZE - 1);
            if (next != buffer_tail) {
                input_buffer[buffer_head] = c;
                buffer_head = next;
            }
        }
    }
    outb(0x20, 0x20);
}

void input(char* output, int len) {
    volatile unsigned short* video = (unsigned short*)0xB8000;
    int i = 0;
    while (1) {
        asm("sti");
        char c = getchar();
        print_char('4');
        if (c == '\b') {
            if (i > 0 && cursor.col > 0) {
                int pos = cursor.row * 80 + (cursor.col - 1);
                video[pos] = (0x0F << 8) | ' ';
                setCursor(cursor.row, cursor.col - 1);
                i--;
            }
        }
        else if (c == '\n') {
            print_char(c);
            break;
        }
        else {
            print_char(c);
            if (i < len - 1) {
                output[i++] = c;
            }
        }
    }
    output[i] = '\0';
}

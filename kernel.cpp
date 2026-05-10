#include "yellowfishAPI.h"

#define uint16_t unsigned short
#define uint32_t unsigned int
#define uint8_t unsigned char

extern "C" void kernel_main() __attribute__((section(".text.start")));

extern "C" void kernel_main() {
    volatile unsigned short* video = (unsigned short*)0xB8000;

    clear_screen();
    print("YF OS Ready\n");

    idt_init();

    char c;

    while (1) {
        asm("sti");
        c = getchar();
        if (c == '\b') {
            video[getRow() * 80 + getCol() - 1] = (0x0F << 8) | ' ';
            setCursor(getCol() - 1,getRow());
        }
        else {
            print_char(c);
        }
    }
}
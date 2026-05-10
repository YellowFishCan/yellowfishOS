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

    char c[256];

    input(c, 256);
    print(c);
    
    while (1);
}

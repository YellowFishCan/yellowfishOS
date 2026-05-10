#ifndef VIDEO_H
#define VIDEO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "port.h"

#define uint16_t unsigned short
#define uint32_t unsigned int
#define uint8_t unsigned char

extern Cursor cursor;

void useCursor();

void setCursor(int row, int col);

int getCol();

int getRow();

void scroll();

void clear_screen();

void print_char(char c);

void print(const char* msg);

//keyboard input

#define BUFFER_SIZE 256

extern char input_buffer[BUFFER_SIZE];
extern int buffer_head;
extern int buffer_tail;

void int80_handler();

void set_idt_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t type_attr);

void keyboard_handler_wrapper();

void dummy_handler();

void keyboard_handler() __attribute__((naked));

void idt_init();

void keyboard_handler_c();

char getchar();

void input(char* output, int len);

#ifdef __cplusplus
}

#endif

#endif

ENTRY(kernel_main)
SECTIONS
{
  . = 0x10000;
  .text.start : { *(.text.start) }
  .text : { *(.text) }
  .data : { *(.data) }
  .bss : { *(.bss) }
}
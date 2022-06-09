OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
  flash (rxai!w) : ORIGIN = 0x00002000, LENGTH = 16K
  ram (wxa!ri) : ORIGIN = 0x0000B000, LENGTH = 2K
}


SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 256;

  .init           :
  {
    KEEP (*(SORT_NONE(.init)))
  } >flash AT>flash

  .ilalign         :
  {
    . = ALIGN(4);
    PROVIDE( _flash_lma = . );
  } >flash AT>flash

  .ialign         :
  {
    PROVIDE( _flash = . );
  } >flash AT>flash

  .text           :
  {
    *(.text .text.*)
  } >flash AT>flash

  .rdata         :
  {
    *(.rdata)
    *(.rodata .rodata.*)
  } >flash AT>flash

  . = ALIGN(4);
  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);
  PROVIDE( _eflash = . );

  .lalign         :
  {
    . = ALIGN(4);
    PROVIDE( _data_lma = . );
  } >flash AT>flash

  .dalign         :
  {
    . = ALIGN(4);
    PROVIDE( _data = . );
  } >ram AT>flash

  .data          :
  {
    PROVIDE( __global_pointer$ = . + 0x200 );
    *(sram)
    *(.data .data.*)
    . = ALIGN(8);
  } >ram AT>flash

  . = ALIGN(4);
  PROVIDE( _edata = . );
  PROVIDE( edata = . );

  PROVIDE( _fbss = . );
  PROVIDE( __bss_start = . );
  .bss            :
  {
    *(.sbss*)
    *(.bss .bss.*)
    *(COMMON)
    . = ALIGN(4);
  } >ram AT>ram

  . = ALIGN(8);
  PROVIDE( _end = . );
  PROVIDE( end = . );

  .stack ORIGIN(ram) + LENGTH(ram) - __stack_size :
  {
    PROVIDE( _heap_end = . );
    . = __stack_size;
    PROVIDE( _sp = . );
  }
}

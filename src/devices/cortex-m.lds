SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;
    } >FLASH =0xff

    .data : AT(_sidata)
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _edata = .;
    } >RAM =0

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    .dmamem (NOLOAD) :
    {
        . = ALIGN(4);
        *(.dmamem)
        *(.dmamem.*)
        . = ALIGN(4);
    } >DMAMEM

    /* Added, so that it can compile with arm-none-eabi-gcc. */
    /DISCARD/ :
    {
        *(.ARM.exidx*)
    }

    /* Stack placed at the end of RAM, aligned to 8 bytes.
       SP register points to the end of the stack */ 
    _estack = (ORIGIN(RAM) + LENGTH(RAM)) & 0xFFFFFFF8;

    /* Throw an error if there is not enough space for the stack and heap */
    ASSERT(_ebss + _minimum_stack_and_heap_size <= _estack,
           "There is not enough space in RAM for stack and heap.")

    /* End of the heap, used by sbrk in libc. */
    PROVIDE(end = _ebss);
    /* PROVIDE(_end = _ebss); */

    /* Extra debugger information. */

    /* Remove std lib debugging info */
    DISCARD :
    {
        libc.a(*)
        libm.a(*)
        libgcc.a(*)
    }

    /* Keep all other debugger information. */
    .stab            0 : {*(.stab)}
    .stabstr         0 : {*(.stabstr)}
    .stab.excl       0 : {*(.stab.excl)}
    .stab.exclstr    0 : {*(.stab.exclstr)}
    .stab.index      0 : {*(.stab.index)}
    .stab.indexstr   0 : {*(.stab.indexstr)}
    .comment         0 : {*(.comment)}
    .debug           0 : {*(.debug)}
    .line            0 : {*(.line)}
    .debug_srcinfo   0 : {*(.debug_srcinfo)}
    .debug_sfnames   0 : {*(.debug_sfnames)}
    .debug_aranges   0 : {*(.debug_aranges)}
    .debug_pubnames  0 : {*(.debug_pubnames)}
    .debug_info      0 : {*(.debug_info .gnu.linkonce.wi.*)}
    .debug_abbrev    0 : {*(.debug_abbrev)}
    .debug_line      0 : {*(.debug_line)}
    .debug_frame     0 : {*(.debug_frame)}
    .debug_str       0 : {*(.debug_str)}
    .debug_loc       0 : {*(.debug_loc)}
    .debug_macinfo   0 : {*(.debug_macinfo)}
    .debug_weaknames 0 : {*(.debug_weaknames)}
    .debug_funcnames 0 : {*(.debug_funcnames)}
    .debug_typenames 0 : {*(.debug_typenames)}
    .debug_varnames  0 : {*(.debug_varnames)}
}

OUTPUT_ARCH(mips)
ENTRY(_start)

SECTIONS
{
  .text : {
        *(.text)
	*(.rodata)
	*(.rodata1)
	*(.rodata.str1.4)
        }

  .reginfo : { *(.reginfo) }

  .data : {
	*(.data)
	}

  .bss  : {
	*(.dynbss)
	*(.bss)
  }
}

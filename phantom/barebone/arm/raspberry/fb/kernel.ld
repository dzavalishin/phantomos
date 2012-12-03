SECTIONS
{
	/* Have the initial code whehre the processor starts */
	OVERLAY 0x0 : AT (0x0000)
	{
		start
		{
			start.o(.init)
		}
	}
	
	/* Have the rest somewhere after where the GPU scribbles stuff */
	OVERLAY 0x8000 : AT (0x8000)
	{
		text
		{
			*(.text)
			*(.rodata)
			*(.data)
		}
	}
	
	/*
	 * There's also some junk...
	 * We don't care about this, other than to keep it out of our way...
	 * We have this junk removed by objcopy
	 */
	junk :
	{
		*(*) /* Remove the junk that we don't need or want */
	}
}

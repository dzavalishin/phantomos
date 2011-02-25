#define CP_IDFIELD 0xCB000000

#define SERIAL_BASE 0x16000000
#define SERIAL_FLAG_REGISTER 0x18
#define SERIAL_BUFFER_FULL (1 << 5)

#define MEM(___a) *((int *)___a)
 
void putc (char c)
{
    /* Wait until the serial buffer is empty */
    while (*(volatile unsigned long*)(SERIAL_BASE + SERIAL_FLAG_REGISTER) 
                                       & (SERIAL_BUFFER_FULL));
    /* Put our character, c, into the serial buffer */
    *(volatile unsigned long*)SERIAL_BASE = c;
}
 
void puts (const char * str)
{
    while (*str)
    {
        if(*str == '\n' )
            putc('\r');
        putc (*str++);
    }
}


void nibble(const unsigned char b)
{
    unsigned char  c = b & 0x0f;
    if (c>9) c += 'A'-10;
    else c += '0';
    putc(c);
} 

void puthex(int b)
{
    nibble(b>>28);
    nibble(b>>24);

    nibble(b>>20);
    nibble(b>>16);

    nibble(b>>12);
    nibble(b>>8);

    nibble(b>>4);
    nibble(b);

    putc('\n');
    putc('\r');

}







 
int main (void)
{
    puts ("hello, world!\n");

	//*((int*)0x900F0020) = 0x80;
    puts ("Board ID: ");
    puthex(MEM(CP_IDFIELD));

    puts ("Will init LCD!\n");

    volatile unsigned int *lp = (void *)0xC0000000;
    //volatile unsigned char *sp = (void *)0xA4000100;
    volatile unsigned int *sp = (void *)0x00200000;

#if 1
    lp[0x10>>2] = (int)sp;

    lp[0x1C>>2] =
        1 | // controller enable
        (2 << 1) | // 4 bpp
        0
		;

	MEM(0x10000014) = 0xA05F;
	MEM(0x1000001C) = 0x12C11;

    puts ("A\n");
	MEM(0xC0000000) = 0x3F1F3F9C;
	MEM(0xC0000004) = 0x080B61DF;
	MEM(0xC0000008) = 0x067F3800;

    puts ("B\n");
	MEM(0xC0000010) = (int)sp;
	MEM(0xC0000014) = (int)sp;

    puts ("C\n");
	//MEM(0xC000001C) = 0x1829; // 16 bpp
	MEM(0xC000001C) = 0x182B; // 24 bpp
    //puts ("D\n");
	//MEM(0x1000000C) = 0x3e005;

#endif	

    puts ("Done init LCD!\n");
	
	puthex(lp[0]);
	puthex(lp[1]);
	puthex(lp[2]);
	puthex(lp[3]);
	puthex(lp[4]);
	puthex(lp[5]);
	puthex(lp[6]);
	puthex(lp[7]);
	puthex(lp[8]);
	puthex(lp[9]);

	puthex(lp[11]);
	puthex(lp[12]);


    int i;
    for( i = 0; i < 10000; i++ )
        sp[i] = (i&4) ? 0xFF0000 : 0x00FF;

    return 0;
}

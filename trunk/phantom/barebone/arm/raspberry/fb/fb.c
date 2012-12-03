#include "gpio.h"
#include "kernel.h"

#include "chars/chars_pixels.h"

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

/* **** For BCM2835 specific, go down to the section titled "BCM2835 Specific Stuff" **** */

/* Frame buffer data that we use (not BCM2835 specific) */
struct FrameBufferCharacter
{
	char c;
	unsigned char bgr;
	unsigned char bgg;
	unsigned char bgb;
	unsigned char fgr;
	unsigned char fgg;
	unsigned char fgb;
};

struct FrameBufferInfo
{
	/* Stuff about the pixel frame buffer */
	unsigned int width;
	unsigned int height;
	unsigned int pitch; //BCM2835 has this separate, so we use this instead of witdh
	volatile unsigned char *pointer;
	
	/* Stuff about the text frame buffer */
	struct FrameBufferCharacter *cb;
	unsigned int line_pos; //The position within the line of the cursor
	unsigned char *charpix; //Character pixels
	unsigned int xoffs; //x offset
	unsigned int yoffs; //y offset
	unsigned int cbwidth; //width in characters
	unsigned int cbheight; //height in characters
};

struct FrameBufferInfo fbinfo;

static void SetStandardColourChar(char c, struct FrameBufferCharacter *fbc)
{
	fbc->c = c;
	fbc->bgr = 255;
	fbc->bgg = 255;
	fbc->bgb = 255;
	fbc->fgr = 0;
	fbc->fgg = 0;
	fbc->fgb = 0;
}

//Perhaps use cat to get the characters into a known location that the linker can't mess with?
static void UpdateFb()
{
	for (unsigned int cy = 0; cy < fbinfo.cbheight; cy++){
		for (unsigned int cx = 0; cx < fbinfo.cbwidth; cx++){
			unsigned int char_offset = (cy * fbinfo.cbwidth) + cx;
			unsigned int fb_bx = (cx * CHAR_WIDTH) + fbinfo.xoffs;
			unsigned int fb_by = (cy * CHAR_HEIGHT) + fbinfo.yoffs;
			
			unsigned int char_index = fbinfo.cb[char_offset].c;
			unsigned int bgr = fbinfo.cb[char_offset].bgr;
			unsigned int bgg = fbinfo.cb[char_offset].bgg;
			unsigned int bgb = fbinfo.cb[char_offset].bgb;
			unsigned int fgr = fbinfo.cb[char_offset].fgr;
			unsigned int fgg = fbinfo.cb[char_offset].fgg;
			unsigned int fgb = fbinfo.cb[char_offset].fgb;
			
			for (unsigned int y = 0; y < CHAR_HEIGHT; y++){
				for (unsigned int x = 0; x < CHAR_WIDTH; x++){
					unsigned int fb_x = fb_bx + x;
					unsigned int fb_y = fb_by + y;
					
					unsigned int v = fbinfo.charpix[char_index * CHAR_WIDTH * CHAR_HEIGHT + y * CHAR_WIDTH + x]; 
					unsigned int fb_offset = fb_y * fbinfo.pitch + fb_x*3;
					
					unsigned char r = (bgr * (255 - v) + fgr * v)/255;
					unsigned char g = (bgg * (255 - v) + fgg * v)/255;
					unsigned char b = (bgb * (255 - v) + fgb * v)/255;
					
					fbinfo.pointer[fb_offset] = r;
					fbinfo.pointer[fb_offset + 1] = g;
					fbinfo.pointer[fb_offset + 2] = b;
				}
			}
		}
	}
	
	MemoryBarrier();
}

static void ClearScreen()
{
	for (unsigned int i = 0; i < fbinfo.cbwidth*fbinfo.cbheight; i++){
		SetStandardColourChar(0, &fbinfo.cb[i]);
	}
}

static void PrintChar(char c)
{
	if ('\n' == c){
		for (unsigned int y = 0; y < fbinfo.cbheight - 1; y++){
			for (unsigned int x = 0; x < fbinfo.cbwidth; x++){
				fbinfo.cb[y * fbinfo.cbwidth + x] = fbinfo.cb[(y + 1) * fbinfo.cbwidth + x];
			}
		}
		for (unsigned int x = 0; x < fbinfo.cbwidth; x++){
			SetStandardColourChar(0, &fbinfo.cb[(fbinfo.cbheight - 1) * fbinfo.cbwidth + x]);
		}
		fbinfo.line_pos = 0;
		return;
	}
	else if (fbinfo.line_pos == fbinfo.cbwidth){
		PrintChar('\n');
	}
	
	SetStandardColourChar(c, &fbinfo.cb[(fbinfo.cbheight - 1) * fbinfo.cbwidth + fbinfo.line_pos]);
	fbinfo.line_pos++;
}

void PrintString(const char *str)
{
	for (unsigned int i = 0; str[i]; i++){
		PrintChar(str[i]);
	}
	UpdateFb();
}


/*
	* ************************************************************************ *
	* ****                     BCM2835 Specific Stuff                     **** *
	* ************************************************************************ *
	* This is where things get interesting (and specific to the BCM2835).
	* Most of this was worked out by reading the Linux source code (mostly
	* drivers/video/bcm2708_fb.c and arch/arm/mach-bcm2708/) and experimentation.
	* 
	* 
	* **** Basic procedure to get stuff on screen ****
	* The basic procedure to get a frame buffer is:
	* 1) Set up a structure with the frame buffer specification (resolution, etc)
	* 2) Tell the GPU about this structure by writing to the mailbox
	* 3) Wait by reading from the mailbox for the GPU to modify this structure
	* 4) Write to the frame buffer at the pointer we got in stage 3
	* Only step 4 is required for subsequent writes to the frame buffer. Currently,
	* I do not know how to enable the HDMI output, so this will always operate the
	* composite, and not the HDMI.
	* 
	* 
	* **** Mailbox operations ****
	* Read/write operatitons on the mailbox consist of transfering data via a
	* 32 bit register. 28 bits of this 32 bit register are the data to be sent
	* to the receiver, while the lower 4 bits specify the channel (channel 1 is
	* the frame buffer, but there are others).
	* 
	* To send data via the mailbox:
	* 1) Wait for space in the mailbox
	* 2) Write ((data << 4) || channel) to the write register TODO: Make implementation match
	* 
	* To receive data via the mailbox:
	* 1) Wait for the mailbox to be non-empty
	* 2) Execute a memory barrier
	* 3) Read from the read register
	* 4) Check the lowest 4 bits of the read value for the correct channel
	* 5) If the channel is not the one we wish to read from (i.e: 1), go to step 1
	* 6) Return the read value >> 4 TODO: Make implementation match
	* Note: This will not work if we're interested in reading from more than one
	* 		channel as it does not handle the reception of other channels' data
	* 
	* 
	* **** Memory mapped registers ****
	* The bus address for the mailbox memory mapped registers is 0x7E00B880.
	* This corresponds to an ARM physical address of 0x2000B880 (the address we
	* use from the ARM processor, and hence here). We use three registers from
	* the mail box:
	* - The read register for mailbox 0 at offset 0x00
	* - The status register for mailbox 0 at offset 0x1C
	* - The write register for mailbox 0 at offset 0x20 (this is actually the read
	* 	register for mailbox 1).
	* 
	* 
	* **** Notes ****
	* - The address of the frame buffer must be at least a multiple of 16 (in
	* 	order to be accurately transmitted in the 28 bits available in the
	* 	mailbox)
	* - The 32 bit value we actually send over the mailbox (including the channel)
	* 	is (ADDRESS | 1) where ADDRESS is the address of the structure. This is
	* 	equivalent to sending as the data (ADDRESS >> 4) (remember we do data << 4)
	* - This works if we set vwidth = width, vheight = height, x = 0, y = 0.
	* - I haven't managaged to make anything but 24 bit depth work, however the
	* 	Linux source seems to use 16 bit?!
	* - Sometimes the procedure described to get stuff on the screen doesn't
	* 	work first time. I've hacked around this by repeating until it does work.
	* - The two conditions for successfully acquiring a frame buffer are:
	* 	- The data read from the mailbox (with the 4 least significant bits set
	* 	  to zero) is 0 (or 1 including the channel)
	* 	- The pointer in the structure is non-zero after the mailbox read
	* - Once we have the frame buffer, we can just write to it. The pixels (in
	* 	24 bit mode) are RGB ordered by y then x coordinate. The address of a
	* 	subpixel is given by: y * pitch + x * 3 + rgb_channel, where rgb_channel
	* 	is 0 for red, 1 for green, and 2 for blue.
*/

#define MAIL_BASE 0xB880 /* This is the base address for the mailbox registers
	(actually, there's more than one mailbox, but this is the one we care about) */

/* Registers from mailbox 0 that we use */
#define MAIL_READ 0x00 /* We read from this register */
#define MAIL_WRITE 0x20 /* This is where we write to; it is actually the read/write of the other mailbox */
#define MAIL_STATUS 0x18 /* Status register for this mailbox */
//#define MAIL_CONFIG 0x1C - we don't actually use this, but it exists

//This bit is set in the status register if there is no space to write into the mailbox
#define MAIL_FULL 0x80000000
//This bit is set if there is nothing to read from the mailbox
#define MAIL_EMPTY 0x40000000

#define MAIL_FB 1 /* The frame buffer uses channel 1 */

struct Bcm2835FrameBuffer
{
	uint32_t width; //Width of the frame buffer (pixels)
	uint32_t height; //Height of the frame buffer
	uint32_t vwidth; //Simplest thing to do is to set vwidth = width
	uint32_t vheight; //Simplest thing to do is to set vheight = height
	uint32_t pitch; //GPU fills this in; set to zero
	uint32_t depth; //Bits per pixel; set to 24
	uint32_t x; //Offset in x direction. Simplest thing to do is set to zero
	uint32_t y; //Offset in y direction. Simplest thing to do is set to zero
	uint32_t pointer; //GPU fills this in to be a pointer to the frame buffer
		//I find this is usually (always?) 0x4F...TODO: Finish
	uint32_t size; //GPU fills this in //TODO: Finish
};

static uint32_t mbox_read()
{
	uint32_t r = 0;
	do {
		while (ReadMmReg32(MAIL_BASE, MAIL_STATUS) & MAIL_EMPTY);//wait for data
		r = ReadMmReg32(MAIL_BASE, MAIL_READ); //Read the data
	} while ((r & 0xF) != MAIL_FB); //Loop until we received something from the
		//frame buffer channel
	return r & 0xFFFFFFF0;
}

static void mbox_write(uint32_t v)
{
	while (ReadMmReg32(MAIL_BASE, MAIL_STATUS) & MAIL_FULL); //wait for space
	//Write the value to the frame buffer channel
	WriteMmReg32(MAIL_BASE, MAIL_WRITE, MAIL_FB | (v & 0xFFFFFFF0));
}

static int TryInitFb()
{
	//Some (or even all?) of these memory barriers can probably be omitted safely
	MemoryBarrier();
	
	//We need to put the frame buffer structure somewhere with the lower 4 bits zero.
	//2^22 is a convenient place not used by anything, and with sufficient alignment
	volatile struct Bcm2835FrameBuffer *fb = (volatile struct Bcm2835FrameBuffer *)(1 << 22);
	//See the comments for Bcm2835FrameBuffer
	fb->width = 640;
	fb->height = 480;
	fb->vwidth = fb->width;
	fb->vheight = fb->height;
	fb->pitch = 0;
	fb->depth = 24;
	fb->x = 0;
	fb->y = 0;
	fb->pointer = 0;
	fb->size = 0;
	
	MemoryBarrier();
	mbox_write(ArmToVc((void *)fb)); //Tell the GPU the address of the structure
	//memory barrier is in the register read/write functions
	MemoryBarrier(); //also an explicit one we probably don't need
	uint32_t r = mbox_read(); //Wait for the GPU to respond, and get its response
	MemoryBarrier();
	
	if (r){ //If r != 0, some kind of error occured
		WriteGpio(17);
		return -1;
	}
	
	if (!fb->pointer){ //if the frame buffer pointer is zero, an error occured
		WriteGpio(18);
		return -2;
	}
	
	//Set up our frame buffer information
	fbinfo.width = fb->width;
	fbinfo.height = fb->height;
	fbinfo.pitch = fb->pitch;
	fbinfo.pointer = (volatile unsigned char *)fb->pointer;
	
	//2^23 is just a convenient space not used by anything
	fbinfo.cb = (struct FrameBufferCharacter *)(1 << 23);
	fbinfo.line_pos = 0;
	fbinfo.charpix = (unsigned char *)chars_pixels;
	
	fbinfo.xoffs = 0; //It works in the x direction nicely (no overscanning)
	fbinfo.yoffs = CHAR_HEIGHT; //cope with overscanning by adding 1 char border
	fbinfo.cbwidth = fb->width / CHAR_WIDTH;
	fbinfo.cbheight = (fb->height / CHAR_HEIGHT) - 2; //add 1 char border at the top and bottom
	
	return 0;
}

void InitFb()
{
	while (TryInitFb()); //Keep trying to make the frame buffer until it works
	ClearScreen();
	UpdateFb();

	volatile char *c = (volatile char *)fbinfo.pointer;
	int cnt = 5000;
	while( cnt-- ) *c++ = 0;
}

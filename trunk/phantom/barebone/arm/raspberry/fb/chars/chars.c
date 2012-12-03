#include <stdio.h>
#include <string.h>

static const char *char_index =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789)!\"$%^&*( _+-=\x01\02"
	"{}:@~|<>?[];'#\\,./`";

static const unsigned int char_width = 8;
static const unsigned int char_height = 16;
static const unsigned int char_cols = 26;
static const unsigned int char_rows = 4;

void CopyPixel(unsigned char *src, unsigned char *dst,
	unsigned int src_width, unsigned int src_x, unsigned int src_y,
	unsigned int dst_width, unsigned int dst_x, unsigned int dst_y)
{
	dst[dst_y * dst_width + dst_x] = (unsigned char)src[src_y * src_width + src_x];
}

int BinMap()
{
	unsigned int bytes = char_width * char_height * char_cols * char_rows;
	unsigned char in[bytes];
	if (fread(in, 1, bytes, stdin) != bytes){
		fprintf(stderr, "Could not read from stdin\n");
		return 1;
	}
	
	bytes = char_width * char_height * 256;
	unsigned char out[bytes];
	
	//start with all being background
	for (unsigned int i = 0; i < bytes; i++){
		out[i] = 0;
	}
	
	//Set the null character
	for (unsigned int i = 0; i < char_height * char_width; i++){
		out[i] = 0xFF;
	}
	
	//iterate through all the characters in the index
	for (unsigned int i = 0; char_index[i]; i++){
		unsigned int in_cx = (i % char_cols); /* Character X */
		unsigned int in_cy = (i / char_cols); /* Character Y */
		unsigned int c = char_index[i];
		
		for (unsigned int y = 0; y < char_height; y++){
			for (unsigned int x = 0; x < char_width; x++){
				CopyPixel(
					in, /* src */
					out, /* dst */
					char_width * char_cols, /* src_width */
					in_cx * char_width + x, /* src_x */
					in_cy * char_height + y, /* src_y */
					char_width, /* dst_width */
					x, /* dst_x */
					char_height*c + y /* dst_y */
				);
			}
		}
	}
	
	if (fwrite(out, 1, bytes, stdout) != bytes){
		fprintf(stderr, "Could not write to stdout\n");
		return 1;
	}
	
	return 0;
}

int ToC()
{
	printf("const char *chars_pixels = ");
	for (unsigned int i = 0; i < 256; i++){
		/*if (i){
			printf(",");
		}*/
		printf("\n\t/* 0x%02x (%c) */", i, (((i >= 0x20) && (i < 0x7e)) ? (char)i : ' '));
		
		for (unsigned int y = 0; y < char_height; y++){
			/*if (y){
				printf(",");
			}*/
			printf("\n\t\"");
			
			for (unsigned int x = 0; x < char_width; x++){
				/*if (x){
					printf(", ");
				}*/
				
				unsigned char c = 0;
				if (1 != fread(&c, 1, 1, stdin)){
					fprintf(stderr, "Could not read from stdin\n");
				}
				
				unsigned int n = c;
				//printf("0x%02x", n);
				printf("\\x%02x", n);
			}
			
			printf("\"");\
		}
	}
	
	printf(";\n");
	return 0;
}

int main(int argc, const char **argv)
{
	if (2 != argc){
		fprintf(stderr, "Invalid arguments");
	}
	
	if (!strcmp("-b", argv[1])){
		return BinMap();
	}
	else if (!strcmp("-c", argv[1])){
		return ToC();
	}
	
	fprintf(stderr, "Argument \"%s\" invalid", argv[1]);
	return 1;
}

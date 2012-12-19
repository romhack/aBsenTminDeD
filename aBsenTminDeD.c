/*
http://creativecommons.org/publicdomain/zero/1.0/
To the extent possible under law, Griever has waived all copyright and related or neighboring rights to aBsenTminDeD. 
This work is published from:  Russian Federation.
*/

#include <stdio.h>
typedef unsigned char  u8;
typedef unsigned short u16;
#define NES_HDR_SIZE 0x10
#define TEXT_BANK_OFFSET 0x10000 + NES_HDR_SIZE

//69 symbols can be encoded
#define TREE_SIZE 0x45
//these are hardcoded
#define TREE_LEFT_OFFS 0xB5AA + TEXT_BANK_OFFSET
#define POINTER_TABLE_OFFS 0xB67A + TEXT_BANK_OFFSET
//14 pointers and text blocks
#define POINTER_TABLE_SIZE 13
#define TEXT_OFFS 0xB696 + TEXT_BANK_OFFSET
//charcodes:
#define END_OF_SCREEN 0xFD
#define END_OF_MSG 0xFF
//each text block, which pointer points to, has 4 screens, terminated by </SCREEN>
#define MAX_SCREEN_IN_BLOCK 4
//each screen has different amount of messages, terminated by </MSG>

//level strings begins from 11th text block, 4th screen (level) and up to the text end
#define LEVEL_STRING_BLOCK 10
#define LEVEL_STRING_SCREEN 3
#define LEVEL_STRING_INDEX LEVEL_STRING_BLOCK*MAX_SCREEN_IN_BLOCK + LEVEL_STRING_SCREEN


FILE* f_input;
u8 tree_left[TREE_SIZE], tree_right[TREE_SIZE];//huffman decoding trees
u8 charmap[TREE_SIZE];
u16 pointer_table[POINTER_TABLE_SIZE];
int global_screencount = 0, global_charcount = 0; //костыль for finding level string




void print_decoded(char chr)//decode table and special chars processing
{
int i, spaces, lines, ppu_chunk_size;

  switch ((u8)chr)
	{
		case END_OF_MSG:
			printf("<\\MSG>\n");
			break;
		case END_OF_SCREEN:
			printf("<\\SCREEN>\n");
			break;
		default:
		
			if (global_screencount >= LEVEL_STRING_INDEX)//level strings process first 2 bytes (or 1) as PPU pointer. Last char has HIbit risen - terminate string
			{
				ppu_chunk_size = (global_screencount == LEVEL_STRING_INDEX) ? 2 : 1;

				if (global_charcount < ppu_chunk_size)
					printf("<PPU:%02x>", (u8)chr);			
				else
					printf("%c", chr & 0x7F);//last char has HIbit - no use for decompress
			}
			
			else//normal message
			{
		
				if ((chr & 0x80) != 0)//Hi bit in ASCII char - means special positioning symbol
				{
					
					lines = (chr & 0x7F) >> 5; //Y coord 2 higher bits
					for (i = 0; i < lines; i++)
						printf("\n");				
					
					spaces = chr & 0x1F; //X coord 5 lower bits
					for (i = 0; i < spaces; i++)
						printf(" ");
				
				}
				else
					printf("%c", chr);	
			}
		
	}
}

void unpack_text_block (u16 ptr)
{
int i;
int first_byte_flag = 0;
u8 offset = 0, chr, byte;

int startbits, bitcount = 0, screencount = 0;

	startbits = (ptr >> 8) & 7;//3 low bits of HI byte of the ptr is startbits
	ptr = (ptr & 0xFF) | ((ptr >> 3) & 0xFF00);// rest 5 bits of HI byte is actually HI byte.


	fseek(f_input, ptr + TEXT_OFFS, SEEK_SET);
	while(screencount < MAX_SCREEN_IN_BLOCK)
	{
		
		byte = fgetc(f_input);//if first byte, concern starting bits position, else process full byte
		if (first_byte_flag == 0)
		{
			bitcount =   startbits;
			byte <<= 7 - startbits; 
			first_byte_flag++;
		}
		else
			bitcount = 7;
		

		for (i = 0; i <= bitcount; i++) 
		{		
			offset = byte & 0x80 ? tree_right[offset] : tree_left[offset];			
			byte <<= 1;
			if (offset >= 0x80)
			{
				chr = charmap [offset - 0x80];
				print_decoded(chr);
				global_charcount++;
				offset = 0;//return to trees root
				if (chr == END_OF_SCREEN)
				{
					screencount++;
					global_screencount++;
					global_charcount = 0;
					if (screencount >= MAX_SCREEN_IN_BLOCK)//ptr for block of 4 messages, next block will be pointed by next ptr
						break;
				}				
			}			
		}		
	}
}

int main (int argc, char *argv[])
{
int i;
char * fname;
	fname = (argc == 1)? "Battletoads & Double Dragon - The Ultimate Team (U).nes": argv[1];

	
	if ((f_input = fopen( fname, "rb")))
	{
		fseek(f_input, TREE_LEFT_OFFS, SEEK_SET);
		fread( tree_left, sizeof(u8), TREE_SIZE, f_input );
		fread( tree_right, sizeof(u8), TREE_SIZE, f_input );//right tree right after left
		fread( charmap, sizeof(u8), TREE_SIZE, f_input );//charmap after trees	
		fseek(f_input, POINTER_TABLE_OFFS, SEEK_SET);
		fread( pointer_table, sizeof(u16), POINTER_TABLE_SIZE, f_input );
		for (i = 0; i < POINTER_TABLE_SIZE; i++)
			unpack_text_block(pointer_table[i]);
		
		fclose(f_input);
	}
	else//IO error
		printf("Could not open file: \"%s\"", fname);
	getchar();

return 0;
}

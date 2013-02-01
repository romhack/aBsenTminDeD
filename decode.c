#include "decode.h"
#include <stdio.h>
#include <strings.h>


FILE* f_in, *f_out;
u8 tree_left[TREE_SIZE], tree_right[TREE_SIZE];//huffman decoding trees
u8 charmap[TREE_SIZE+1];//n nodes can code n+1 leafs
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

void decode_text_block (int textBase, u16 ptr, int binaryFlag)
{
int i;
int first_byte_flag = 0;
u8 offset = 0, chr, byte;

int startbits, bitcount = 0, screencount = 0;

	startbits = (ptr >> 8) & 7;//3 low bits of HI byte of the ptr is startbits
	ptr = (ptr & 0xFF) | ((ptr >> 3) & 0xFF00);// rest 5 bits of HI byte are actually HI byte.


	fseek(f_in, textBase+ptr, SEEK_SET);
	while(screencount < MAX_SCREEN_IN_BLOCK)
	{
		
		byte = fgetc(f_in);//if first byte, concern starting bits position, else process full byte
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
			if (offset >= 0x80)//leaf - print decoded char
			{
				chr = charmap [offset - 0x80];
				if (binaryFlag)
					fwrite(&chr,1,sizeof(u8),f_out);
				else
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

int decodeFile (char* fileName, int binaryFlag)//decodes a file (default to screen), if binary flag is set, dumps to file.
{
int i;
char * fname = fileName;

	

	if ((f_in = fopen( fname, "rb")))
	{
		if (binaryFlag)
			f_out = fopen("rawText.bin", "wb");
		fseek(f_in, TREE_LEFT_OFFS, SEEK_SET);
		fread( tree_left, sizeof(u8), TREE_SIZE, f_in);
		fread( tree_right, sizeof(u8), TREE_SIZE, f_in);//right tree right after left
		fread( charmap, sizeof(u8), TREE_SIZE+1, f_in);//charmap after trees
		fread( pointer_table, sizeof(u16), POINTER_TABLE_SIZE, f_in);//pointers after charmap and then compressed text
		int textBase = ftell(f_in);
		for (i = 0; i < POINTER_TABLE_SIZE-1; i++)//last pointer is 0000, don't process
			decode_text_block(textBase, pointer_table[i], binaryFlag);
		
		fclose(f_in);
		if (binaryFlag)
			fclose (f_out);
			printf("Binary dumped to rawText.bin\n");
		getchar();
	}
	else//IO error
		printf("Could not open file: \"%s\"", fname);
return 0;
}
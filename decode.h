#pragma once
#ifndef _DECODE_H
#define _DECODE_H




typedef unsigned char  u8;
typedef unsigned short u16;
#define NES_HDR_SIZE 0x10
#define TEXT_BANK_OFFSET 0x10000 + NES_HDR_SIZE

//69 symbols can be encoded
#define TREE_SIZE 0x45
//text data starts here
#define TREE_LEFT_OFFS 0xB5AA + TEXT_BANK_OFFSET

//14 pointers and text blocks
#define POINTER_TABLE_SIZE 14

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

int decodeFile (char* fileName, int binaryFlag);

#endif
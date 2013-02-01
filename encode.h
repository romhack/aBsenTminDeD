#pragma once
#ifndef _ENCODE_H
#define _ENCODE_H

#include <stdio.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

//defines for htNode
#define LEFT_NODE 0
#define RIGHT_NODE 1



//The Huffman tree node definition
typedef struct _htNode {
	int direction;//left or right node
	int parental_node;//nodecount of node's parent
	u8 symbol;
	struct _htNode *left, *right;
}htNode;

/*
	We "encapsulate" the entire tree in a structure
	because in the future we might add fields like "size"
	if we need to. This way we don't have to modify the entire
	source code.
*/
typedef struct _htTree {
	size_t size;
	htNode *root;
}htTree;

//The Huffman table nodes (linked list implementation)
typedef struct _hlNode {
	u8 symbol;
	char *code;//path to this node - all passed symbols form a string - code.
	struct _hlNode *next;
}hlNode;


typedef struct _hlTable {
	hlNode *first;
	hlNode *last;
}hlTable;

htTree * buildTree(u8* inputString, size_t size);
hlTable * buildTable(htTree *huffmanTree);


void encodeFile(char* fileName);

#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "encode.h"
#include "pQueue.h"
#include "decode.h"

void traverseTree(htNode *treeNode, hlTable **table, int k, char code[256])
{
	//If we reach the end we introduce the code in the table
	if(treeNode->left == NULL && treeNode->right == NULL)
	{
		code[k] = '\0';//null terminated string - we've finished path to one symbol and code string is ended
		hlNode *aux = (hlNode *)malloc(sizeof(hlNode));
		aux->code = (char *)malloc(sizeof(char)*(strlen(code)+1));
		strcpy(aux->code,code);//save code string
		aux->symbol = treeNode->symbol;
		aux->next = NULL;
		if((*table)->first == NULL)
		{
			(*table)->first = aux;
			(*table)->last = aux;
		}
		else
		{
			(*table)->last->next = aux;
			(*table)->last = aux;
		}
		
	}
	
	//We concatenate a 0 for each step to the left
	if(treeNode->left!=NULL)
	{
		code[k]='0';
		traverseTree(treeNode->left,table,k+1,code);
		
	}
	//We concatenate a 1 for each step to the right
	if(treeNode->right!=NULL)
	{
		code[k]='1';
		traverseTree(treeNode->right,table,k+1,code);
		
	}
}

hlTable * buildTable(htTree * huffmanTree)
{
	//We initialize the table
	hlTable *table = (hlTable *)malloc(sizeof(hlTable));
	table->first = NULL;
	table->last = NULL;
	
	//Auxiliary variables
	char code[0x100];
	//k will memories the level on which the traversal is
	int k=0;

	//We traverse the tree and calculate the codes
	traverseTree(huffmanTree->root,&table,k,code);
	return table;
}

htTree * buildTree(u8* inputString, size_t size)
{
u32 i;
	//The array in which we calculate the frequency of the symbols
	//Knowing that there are only 256 posibilities of combining 8 bits
	//(256 ASCII characters)
	int probability[0x100] = {0};
	

	//We consider the symbol as an array index and we calculate how many times each symbol appears
	for(i=0; i < size; i++)
		probability[(unsigned char) inputString[i]]++;

	//The queue which will hold the tree nodes
	pQueue * huffmanQueue;
	initPQueue(&huffmanQueue);
	
	size_t treeSize = 0;
	//We create nodes for each symbol in the string
	for(i=0; i<0x100; i++)
		if(probability[i]!=0)
		{
			htNode *aux = (htNode *)malloc(sizeof(htNode));
			aux->left = NULL;
			aux->right = NULL;
			aux->symbol = (u8) i;
			
			addPQueue(&huffmanQueue,aux,probability[i]);
			treeSize++;
		}


	
	

	//Combine lowest nodes and build a tree
	while(huffmanQueue->size!=1)
	{
		int priority = huffmanQueue->first->priority;
		priority+=huffmanQueue->first->next->priority;

		htNode *left = getPQueue_FO(&huffmanQueue);
		htNode *right = getPQueue_FO(&huffmanQueue);

		htNode *newNode = (htNode *)malloc(sizeof(htNode));
		newNode->left = left;
		newNode->right = right;
		newNode->symbol = 0;

		addPQueue(&huffmanQueue,newNode,priority);
	}

	//We create the tree
	htTree *tree = (htTree *) malloc(sizeof(htTree));
	
	tree->size = treeSize;
	tree->root = getPQueue_FO(&huffmanQueue);
	
	return tree;
}

void writeBinStr(char* encodedStr, const char* fileName)//pack and write binary string into binary file
{
FILE* f_out = fopen(fileName, "wb");
int i;
u8 byte = 0, bitCount = 0;

	for (i = 0; i < strlen(encodedStr); i++)
	{
		if (encodedStr[i] == '1')
			byte |= 1;
		bitCount++;
		if (bitCount == 8)
		{
			fwrite(&byte, 1, sizeof(u8), f_out);
			bitCount = 0;
			byte = 0;		
		}
		byte <<= 1;
	}
	if (bitCount != 8)
	{
		byte <<= 7-bitCount;//allign last byte - rest of lo-bits will be zeroed
		fwrite(&byte, 1, sizeof(u8), f_out);
	}
	fclose(f_out);
}

void encode(hlTable *table, u8 *stringToEncode, size_t size, const char*fileName, u16* ptrTable)//writes string of 0s and 1s to specified file and fills pointer table for text
{
hlNode *traversal;
int i, screenCount = 0;

	
	char* encodedStr = (char*)malloc(sizeof(char));
	encodedStr[0] = '\0';


	for( i= 0; i< size; screenCount++ )
	{
		if (!(screenCount%MAX_SCREEN_IN_BLOCK))//each 4 screens write pointer to block
		{
			u16 ptr = strlen(encodedStr)/8;
			int bits = 7-((strlen(encodedStr))&7); 
			ptr = ((ptr & 0xFF00) << 3) | (ptr & 0xFF);//3 low bits of HI byte of the ptr is startbits; rest 5 bits of HI byte is actually HI byte.
			ptr |= bits <<8;
			ptrTable[screenCount/MAX_SCREEN_IN_BLOCK] = ptr;
			//printf("%4x\n", ptr);
		}
	
		do //encode one screen
		{
			traversal = table->first;
			while(traversal->symbol != stringToEncode[i]) 	//For each element of the string traverse the table
				traversal = traversal->next;				//and once we find the symbol we output the code for it			
			
			size_t len = strlen(encodedStr) + strlen(traversal->code) + 1;//concatenate string with found code of symbol, +1 for NULL-terminator
			encodedStr = (char*)realloc(encodedStr,len);
			strcat(encodedStr, traversal->code);
			
		} while ((u8)stringToEncode[i++] != END_OF_SCREEN);
	}
	//printf("%s", encodedStr);	
	writeBinStr(encodedStr, fileName);//write encoded string to file
	free(encodedStr);
}


void encodeFile(char* fileName)
{



FILE* f_in;
size_t text_size;
u8* text_buffer;

	f_in = fopen(fileName, "rb");
	fseek(f_in, 0, SEEK_END);
	text_size = ftell(f_in);
	fseek(f_in, 0, SEEK_SET);
	text_buffer = (u8*)malloc(text_size);
	fread (text_buffer, text_size, sizeof(u8), f_in);
	fclose(f_in);
	
	//We build the tree depending on the string
	htTree *codeTree = buildTree(text_buffer, text_size);
	
	
	
	
	//Prepare to write charmap and trees files
	FILE* f_charmap = fopen("charmap.bin", "wb");
	
	size_t treeSize = codeTree->size - 1;
	u8* p_treeLeft = (u8*)malloc(treeSize);//left tree has x nodes and x+1 leafs	

	FILE* f_treeLeft = fopen ("treeLeft.bin", "wb");
	u8* p_treeRight = (u8*)malloc(treeSize);
	FILE* f_treeRight = fopen ("treeRight.bin", "wb");
	u8* dstTree;
	

	//Breadth traversal of code tree for writing charmap and trees files	
	pQueue * nodeQueue;//The queue which will hold the tree nodes left to traverse
	initPQueue(&nodeQueue);
	addPQueue_LI(&nodeQueue, codeTree->root, 0, 0);//start from root
	
	int nodeCount = 0, charCount=0;//0-based char index for addressing in charmap
	

	
	while(nodeQueue->size!=0)
	{
		htNode *current_node = getPQueue_FO(&nodeQueue);//pulling a node from queue and process it
		//char dir = (current_node->direction == LEFT_NODE)? 'L':'R';//for debug screen printout
		dstTree = (current_node->direction == LEFT_NODE)? p_treeLeft:p_treeRight;//we will write to appropriate tree

	
		if ((current_node->right == NULL) && (current_node->left == NULL))//Thats a leaf!
		{
			//printf("_%c%02X\n",dir, current_node->symbol); debug
			fwrite (&current_node->symbol, 1, sizeof(u8), f_charmap);//write byte to charmap
			dstTree[current_node->parental_node] = charCount | 0x80;//hibit - is leaf flag
			charCount++;
		}
		else //Thats a node!
		{
			if (current_node == codeTree->root) //do not write tree root
			{
				addPQueue_LI(&nodeQueue, current_node->right, RIGHT_NODE, 0);
				addPQueue_LI(&nodeQueue, current_node->left, LEFT_NODE, 0);
				nodeCount = codeTree->size - 2;//init nodeCount at root: left and right tree: each has x nodes and x+1 leafs, s
			}
			else //thats a usual node - write and add nodes to queue
			{	
				//printf("%c%02X\n", dir, nodeCount); debug
				dstTree[current_node->parental_node] = (u8)nodeCount;
			
				if (current_node->right != NULL)
					addPQueue_LI(&nodeQueue, current_node->right, RIGHT_NODE, nodeCount);		
				
				if (current_node->left != NULL )
					addPQueue_LI(&nodeQueue, current_node->left, LEFT_NODE, nodeCount);				
		
				nodeCount--;
			}
		}
	}
	
	fclose (f_charmap);
	fwrite(p_treeLeft, treeSize, sizeof(u8), f_treeLeft); 
	free(p_treeLeft);
	fclose (f_treeLeft);
	
	fwrite(p_treeRight, treeSize, sizeof(u8), f_treeRight);
	free(p_treeRight);
	fclose (f_treeRight);	
	
	

	
	
	
	//We build the table depending on the Huffman tree
	hlTable *codeTable = buildTable(codeTree);
	
	u16 ptrTable[POINTER_TABLE_SIZE] = {0};

	
	encode(codeTable, text_buffer, text_size, "packedText.bin", ptrTable);//We encode using the Huffman table and write pointer table
	free(text_buffer);
	
	FILE* f_ptr = fopen ("pointers.bin", "wb");
	fwrite(ptrTable, POINTER_TABLE_SIZE, sizeof(u16), f_ptr);	
	fclose (f_ptr);
	
	
	printf("Encoded successfully to binary files.\n");
	getchar();

}
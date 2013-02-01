/*
Decoding/Encoding tool for text in NES game: Battletoads & Double Dragon - The Ultimate Team (U)
Griever.magicteam.net
Huffman encoding part is derived from simple example from http://en.nerdaholyc.com/huffman-coding-on-a-string/


Usage:
aBsenTminDeD -d [sourceRom.nes] [-b]
or
aBsenTminDeD -e [sourceText.bin]

d - decode text from sourceRom and write it to stdout or dump in binary file "rawText.bin" -b is specified
Default sourceRom is "Battletoads & Double Dragon - The Ultimate Team (U).nes"
e - encode text from binary sourceText.bin.
Default sourceText is "rawText.bin"
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "encode.h"
#include "pQueue.h"
#include "decode.h"

int    main (int argc, char **argv)
{

char *fileName="";

if (argc > 1)
{
	//processing arguments
	int c;
	int decodeFlag = 0, encodeFlag = 0, binaryFlag = 0;
	//opterr = 0;    
	while ((c = getopt (argc, argv, "d:be:")) != -1)
	{
	 switch (c)
	   {
	   case 'd':
		 decodeFlag = 1;
		 fileName = optarg;
		 break;
	   case 'e':
		 encodeFlag = 1;
		 fileName = optarg;
		 break;
	   case 'b':
		 binaryFlag = 1;
		 break;
	   case '?':
		if (optopt == 'e')
		{
			fileName = "rawText.bin";
			fprintf (stderr, "Option -e requires a filename. Set to default.\n");
		}
		else if (optopt == 'd')
		{
			fileName = "Battletoads & Double Dragon - The Ultimate Team (U).nes";
			fprintf (stderr, "Option -d requires a filename. Set to default.\n");
		}
		else
			fprintf (stderr, "Unknown option character: %c.\n", optopt);	
	   default:
		 ;
		}
	}
	
	if (decodeFlag)
		decodeFile(fileName, binaryFlag);
	else if (encodeFlag)
		encodeFile(fileName);
	else
	{
		fprintf(stderr,"Cannot find encode/decode flags in arguments.");
		getchar();
		exit (EXIT_FAILURE);
	}	
	
}
else
{
	fprintf(stderr,"Usage: aBsenTminDeD -d fileName [-b] [-e fileName]");
	getchar();
	exit (EXIT_FAILURE);
}  

	return 0;
}
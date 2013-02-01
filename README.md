aBsenTminDeD
============
Decoding/Encoding tool for text in NES game: Battletoads & Double Dragon - The Ultimate Team (U)


Usage:
```
aBsenTminDeD -d [sourceRom.nes] [-b]
```
or
```
aBsenTminDeD -e [sourceText.bin]
```

***d*** - decode text from sourceRom and write it to stdout or dump in binary file "rawText.bin" if -b is specified
Default sourceRom is "Battletoads & Double Dragon - The Ultimate Team (U).nes"
***e*** - encode text from binary sourceText.bin.
Default sourceText is "rawText.bin"

Text in the game is compressed with standard Huffman coding. The tool can decode all text blocks either in stdout (try aBsenTminDeD -d >script.txt) or to raw binary file, where text blocks can be edited with any text editing utility, like Kruptar. Pointers will be recalculated at encoding phase - editor just should not exceed initial text blocks number (terminated by 0xFD).
Text then can be encoded with aBsenTminDeD -e rawText.bin. Tool will generate necessary files, which could be inserted any convenient way back in ROM. I use the scheme, described in run.bat, utilizing my insertion tool insertBin.

For license information please see LICENSE.md
Huffman encoding part is derived from simple example from http://en.nerdaholyc.com/huffman-coding-on-a-string/

ToDo
====
Add gfx/NT compression functions.
_____________________
Griever.Magicteam.net

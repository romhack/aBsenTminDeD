:start
del *.bin



echo off

set romName="Battletoads & Double Dragon - The Ultimate Team (U).nes"
set treeLeftOffset=0x1B5BA
set treeLeftSize=0x45
set treeRightOffset=0x1B5FF
set treeRightSize=0x45
set charmapOffset=0x1B644
set charmapSize=0x46
set pointerOffset=0x1B68A
set pointerSize=0x1C
set textOffset=0x1B6A6
set textSize=0x98a

echo on

aBsenTminDeD -d %romName% -b

aBsenTminDeD -e rawText.bin

insertBin treeLeft.bin %romName% -o %treeLeftOffset% -s %treeLeftSize%
insertBin treeRight.bin %romName% -o %treeRightOffset% -s %treeRightSize%
insertBin charmap.bin %romName% -o %charmapOffset% -s %charmapSize%
insertBin pointers.bin %romName% -o %pointerOffset% -s %pointerSize%
insertBin packedText.bin %romName% -o %textOffset% -s %textSize%

echo "Done"
goto :start
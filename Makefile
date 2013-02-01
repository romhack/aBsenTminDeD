# --- Project details -------------------------------------------------

PROJ    := aBsenTminDeD
TARGET  := $(PROJ)

COBJS   := main.o encode.o pQueue.o decode.o

DEBUG	:= 0

# --- Build defines ---------------------------------------------------

CC      := gcc
LD      := gcc

ifeq ($(DEBUG), 1)
	CFLAGS  := -O0 -ggdb -Wall
else
	CFLAGS  := -Os -s -Wall
endif





.PHONY : build clean run debug

# --- Build -----------------------------------------------------------
# Build process starts here!

build: $(TARGET).exe


# Compile (step 1)

$(PROJ).exe: $(COBJS)
	$(CC) $^ $(CFLAGS) -o $@

$(COBJS) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@


# --- Debug -----------------------------------------------------------

debug: 
	gdb --silent --args $(TARGET).exe -d "bt.nes" -b


# --- Clean -----------------------------------------------------------

clean : 
	@rm -fv $(TARGET).exe
	@rm -fv *.dll
	@rm -fv *.wcx
	@rm -fv *.o

# --- Run -----------------------------------------------------------
# Run app
run: 
	./$(TARGET).exe

#EOF

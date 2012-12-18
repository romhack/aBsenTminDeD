# --- Project details -------------------------------------------------

PROJ    := aBsenTminDeD
TARGET  := $(PROJ)

COBJS   := $(PROJ).o

DEBUG := 0

# --- Build defines ---------------------------------------------------

CC      := gcc
LD      := gcc

ifeq ($(DEBUG), 0)
CFLAGS  := -Os -s -Wall
else
CFLAGS  := -ggdb -O0 -Wall
endif


.PHONY : build clean run

# --- Build -----------------------------------------------------------
# Build process starts here!

build: $(TARGET).exe


# Compile (step 1)

$(PROJ).exe: $(COBJS)
	$(CC) $^ $(CFLAGS) -o $@

$(COBJS) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@



# --- Clean -----------------------------------------------------------

clean : 

	@rm -fv *.dll
	@rm -fv *.wcx
	@rm -fv *.o

# --- Run -----------------------------------------------------------
# Run app
run: 
	

#EOF

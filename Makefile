# EMULATOR 101
# Makefile for projects 
#
#

CC=gcc
CFLAGS=-Wall -g2 -O0 -std=c99
LDFLAGS=
LIBS=

SRC_DIR=src
DISASSEM_SRC_DIR=$(SRC_DIR)/disassem
OBJ_DIR=obj

# Sources, objects, etc
INCLUDES := -I/$(SRC_DIR)
HEADERS := $(wildcard $(SRC_DIR)/*.h)
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)


# Disassembler 
DISASSEM_SRC := $(wildcard $(DISASSEM_SRC_DIR)/*.c)
DISASSEM_OBJ := $(DISASSEM_SRC:$(DISASSEM_SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Build object files 
$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@ $(LIBS)

$(DISASSEM_OBJ): $(OBJ_DIR)/%.o : $(DISASSEM_SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@ $(LIBS)

obj: $(OBJECTS) 

# NOTE: because of the way that I split the disassember and emulator, I have
# this awkward extra object line in the final build
all : obj disassem
	$(CC) $(LDFLAGS) $(INCLUDES) $(OBJECTS) $(OBJ_DIR)/disassem.o -o emu101 $(LIBS)

disassem: $(DISASSEM_OBJ)
	$(CC) $(LFLAGS) $(INCLUDES) $(DISASSEM_OBJ) -o disassem $(LIBS)

.PHONY: clean

clean:
	rm -fv $(OBJ_DIR)/*.o

# Debug 
print-%:
	@echo $* = $($*)



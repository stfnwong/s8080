# EMULATOR 101
# Makefile for projects 
#
#

CC=gcc
CFLAGS=-Wall -g2 -O0 -std=c99
LDFLAGS=
LIBS=

SRC_DIR=src
OBJ_DIR=obj


INCLUDES := -I/$(SRC_DIR)
HEADERS := $(wildcard $(SRC_DIR)/*.h)
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)


# Disassembler 
DISASSEM_SRC_DIR=$(SRC_DIR)/disassem
DISASSEM_SRC := $(wildcard $(DISASSEM_SRC_DIR)/*.c)
DISASSEM_OBJ := $(DISASSEM_SRC:$(DISASSM_SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Build object files 
$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@ $(LIBS)

obj: $(OBJECTS)

all : $(OBJECTS)
	$(CC) $(LDFLAGS) $(INCLUDES) $(OBJECTS) -o emu101 $(LIBS)

disassem: $(DISASSEM_OBJ)
	$(CC) $(LFLAGS) $(INCLUDES) $(DISASSEM_OBJ) -o disassem $(LIBS)

.PHONY: clean

clean:
	rm -fv $(OBJ_DIR)/*.o

# Debug 
print-%:
	@echo $* = $($*)



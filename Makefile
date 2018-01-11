# EMULATOR 101
# Makefile for projects 
#
#

CC=gcc
CFLAGS=-Wall -g2 -O0 
LDFLAGS=
LIBS=

SRC_DIR=src
OBJ_DIR=obj


INCLUDES := -I/$(SRC_DIR)
HEADERS := $(wildcard $(SRC_DIR)/*.h)
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Build object files 
$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@ $(LIBS)

obj: $(OBJECTS)

all : $(OBJECTS)
	$(CC) $(LDFLAGS) $(INCLUDES) $(OBJECTS) -o emu101 $(LIBS)

diss: $(OBJECTS)

.PHONY: clean

clean:
	rm -fv $(OBJ_DIR)/*.o

# Debug 
print-%:
	@echo $* = $($*)



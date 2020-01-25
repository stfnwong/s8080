# EMULATOR 101
# Makefile for projects 
#
#

# Directories
BIN_DIR=bin
SRC_DIR=src
OBJ_DIR=obj
TOOL_DIR=tools

# Flags 
CC=gcc
CFLAGS=-Wall -g2 -O0 -std=c99 -I$(SRC_DIR) 
LDFLAGS=
LIBS=

# Sources, objects, etc
INCLUDES := -I/$(SRC_DIR)
HEADERS := $(wildcard $(SRC_DIR)/*.h)
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Build object files 
$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@ $(LIBS)

$(DISASSEM_OBJ): $(OBJ_DIR)/%.o : $(DISASSEM_SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< $(INCLUDES) -o $@ $(LIBS)

obj: $(OBJECTS) 

# ======== TEST ======== #
TESTS=

# ======== TOOLS ======== #
TOOLS=dis_main  
TOOL_SOURCES := $(wildcard $(TOOL_DIR)/*.c)
TOOL_OBJECTS := $(TOOL_SOURCES:$(TOOL_DIR)/%.c=$(OBJ_DIR)/%.o)

$(TOOL_OBJECTS) : $(OBJ_DIR)/%.o : $(TOOL_DIR)/%.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

$(TOOLS): $(OBJECTS) $(TOOL_OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJ_DIR)/$@.o\
		-o $(BIN_DIR)/$@ $(LIBS) $(TEST_LIBS)

# NOTE: because of the way that I split the disassember and emulator, I have
# this awkward extra object line in the final build
#all : obj disassem
#	$(CC) $(LDFLAGS) $(INCLUDES) $(OBJECTS) $(OBJ_DIR)/disassem.o -o emu101 $(LIBS)
#
#disassem: $(DISASSEM_OBJ)
#	$(CC) $(LFLAGS) $(INCLUDES) $(DISASSEM_OBJ) -o disassem $(LIBS)

.PHONY: clean

all : obj tools

tools: $(TOOLS)

#test: $(TESTS)

clean:
	rm -fv $(OBJ_DIR)/*.o

# Debug 
print-%:
	@echo $* = $($*)



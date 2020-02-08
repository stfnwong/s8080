# EMULATOR 101
# Makefile for projects 
#
# Stefan Wong 

# Directories
BIN_DIR=bin
SRC_DIR=src
OBJ_DIR=obj
TOOL_DIR=tools

# Flags 
CC=gcc
CFLAGS=-Wall -g2 -O0 -std=c11 -I$(SRC_DIR) 
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
TESTS=test_lexer test_token test_opcode test_line_info test_source_info test_assembler
TEST_SOURCES=$(wildcard test/*.c)	
TEST_OBJECTS  := $(TEST_SOURCES:test/%.c=$(OBJ_DIR)/%.o)

$(TEST_OBJECTS): $(OBJ_DIR)/%.o : test/%.c 
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@ 

$(TESTS): $(TEST_OBJECTS) $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJ_DIR)/$@.o\
		-o bin/test/$@ $(LIBS) $(TEST_LIBS)

# ======== TOOLS ======== #
TOOLS=asm8080 dis8080 emu8080  
TOOL_SOURCES := $(wildcard $(TOOL_DIR)/*.c)
TOOL_OBJECTS := $(TOOL_SOURCES:$(TOOL_DIR)/%.c=$(OBJ_DIR)/%.o)

$(TOOL_OBJECTS) : $(OBJ_DIR)/%.o : $(TOOL_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TOOLS): $(OBJECTS) $(TOOL_OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(OBJ_DIR)/$@.o\
		-o $(BIN_DIR)/$@ $(LIBS) $(TEST_LIBS)


# ======== TARGETS ======== #
.PHONY: clean

all : obj tools test

tools: $(TOOLS)

test: $(TESTS)

#test: $(TESTS)

clean:
	rm -fv $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/asm8080
	rm -f $(BIN_DIR)/dis8080
	rm -f $(BIN_DIR)/emu8080

# Debug 
print-%:
	@echo $* = $($*)

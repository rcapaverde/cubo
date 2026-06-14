SHELL=sh

SRC_DIR = src
BIN_DIR = bin/linux

GCC = g++

CCOPTS = -g -O0 -Wall -I$(SRC_DIR) -std=c++14
LDOPTS = -g -O0

build: $(BIN_DIR) $(BIN_DIR)/asm

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/asm: $(BIN_DIR)/asm.o $(BIN_DIR)/assembler.o 
	@echo === Criando ASM
	$(GCC) $(LDOPTS) -o $@ $^

$(BIN_DIR)/asm.o: $(SRC_DIR)/asm.cpp
	$(GCC) $(CCOPTS) -c -o $@ $<

$(BIN_DIR)/assembler.o: $(SRC_DIR)/assembler.cpp
	$(GCC) $(CCOPTS) -c -o $@ $<

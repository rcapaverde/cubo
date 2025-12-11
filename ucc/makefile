SHELL=sh

GCC_DIR ?= c:/MinGW
SRC_DIR = src
BIN_DIR = bin/win32

GCC = $(GCC_DIR)/bin/gcc.exe

CCOPTS = -g -O0 -Wall -std=gnu99 -I$(SRC_DIR) 
LDOPTS = -g -O0 -L$(GCC_DIR)/lib

build: $(BIN_DIR) $(BIN_DIR)/ucc.exe

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/ucc.exe: $(BIN_DIR)/ucc.o $(BIN_DIR)/context.o
	@echo === Criando ucc.EXE
	$(GCC) $(LDOPTS) -o $@ $^

$(BIN_DIR)/ucc.o: $(SRC_DIR)/ucc.c
	$(GCC) $(CCOPTS) -c -o $@ $<

$(BIN_DIR)/context.o: $(SRC_DIR)/context.c
	$(GCC) $(CCOPTS) -c -o $@ $<


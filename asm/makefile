SHELL=sh

GCC_DIR ?= c:/MinGW
SRC_DIR = src
BIN_DIR = bin/win32

GCC = $(GCC_DIR)/bin/g++.exe

CCOPTS = -g -O0 -Wall -I$(SRC_DIR) -std=c++14
LDOPTS = -g -O0 -L$(GCC_DIR)/lib

build: $(BIN_DIR) $(BIN_DIR)/asm.exe

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/asm.exe: $(BIN_DIR)/asm.o $(BIN_DIR)/util.o $(BIN_DIR)/parser.o $(BIN_DIR)/isa3.o \
		$(BIN_DIR)/objfile.o $(BIN_DIR)/exefile.o 
	@echo === Criando asm.EXE
	$(GCC) $(LDOPTS) -o $@ $^

$(BIN_DIR)/asm.o: $(SRC_DIR)/asm.cpp
	$(GCC) $(CCOPTS) -c -o $@ $<

$(BIN_DIR)/parser.o: $(SRC_DIR)/parser.cpp
	$(GCC) $(CCOPTS) -c -o $@ $<

$(BIN_DIR)/isa3.o: $(SRC_DIR)/isa3.cpp
	$(GCC) $(CCOPTS) -c -o $@ $<

$(BIN_DIR)/objfile.o: $(SRC_DIR)/objfile.cpp
	$(GCC) $(CCOPTS) -c -o $@ $<

$(BIN_DIR)/exefile.o: $(SRC_DIR)/exefile.cpp
	$(GCC) $(CCOPTS) -c -o $@ $<

$(BIN_DIR)/util.o: $(SRC_DIR)/util.cpp
	$(GCC) $(CCOPTS) -c -o $@ $<

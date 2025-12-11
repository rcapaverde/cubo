#ifndef EXEFILE_H
#define EXEFILE_H

typedef struct {
    int len;
    unsigned char *code;
} CodeFragment;

typedef struct {
    int start;
    int end;
    std::vector<CodeFragment *> fragments;
} CodeBlock;

typedef struct {
    char *symbol;
    unsigned char *addr;
    int len;
} SymbolReference;

char saveExeFileHex(ObjectFile *object, char *filename);

#endif
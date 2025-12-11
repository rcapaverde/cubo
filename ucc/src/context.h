
#ifndef CONTEXT_H
#define CONTEXT_H

#define MAX_STEPS           16
#define MAX_INSTRUCTIONS    512

typedef struct 
{
    char *name;
    unsigned char signal;
    unsigned char mask;
} CodeStep;

typedef struct CompileMacro
{
    char *filename;
    int lineno;
    char *name;
    int count;
    struct {
        unsigned char signal;
        unsigned char mask;
    } steps[MAX_STEPS];
    struct CompileMacro *next;
} CompileMacro;

typedef struct 
{
    CompileMacro *macros;
} CompileContext;

CompileMacro *compile_appendMacro(CompileContext *context, char *macroName, char *fileName, int lineno);
CompileMacro *compile_getMacro(CompileContext *context, char *macroName);

#endif

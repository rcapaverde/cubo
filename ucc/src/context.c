#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "context.h"

CompileMacro *compile_appendMacro(CompileContext *context, char *name, char *fileName, int lineno)
{
    CompileMacro *macro = malloc(sizeof(CompileMacro));
    memset(macro, 0, sizeof(CompileMacro));
    macro->name = strdup(name);
    macro->filename = strdup(fileName);
    macro->lineno = lineno;

    macro->next = context->macros;
    context->macros = macro;

    return macro;
}

CompileMacro *compile_getMacro(CompileContext *context, char *macroName)
{
    CompileMacro *macro = context->macros;
    while (macro)
    {
        if (strcmp(macro->name, macroName) == 0)
            return macro;
        macro = macro->next;
    }
    return NULL;
}


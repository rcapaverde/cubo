#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include "util.h"
#include "parser.h"
#include "objfile.h"
#include "exefile.h"

static CodeBlock *addCodeBlock(std::vector<CodeBlock *> *segments, int address)
{
    CodeBlock *code_block = (CodeBlock *)malloc(sizeof(CodeBlock));
    memset(code_block, 0, sizeof(CodeBlock));
    code_block->start = address;
    code_block->end = address;
    segments->push_back(code_block);
    return code_block;
}

static CodeFragment *addCodeToBlock(CodeBlock *code_block, int size, unsigned char *bytes)
{
    CodeFragment *fragment = (CodeFragment *)malloc(sizeof(CodeFragment));
    memset(fragment, 0, sizeof(CodeFragment));
    fragment->len = size;
    code_block->end += size;
    fragment->code = bytes;
    code_block->fragments.push_back(fragment);
    return fragment;
}

static void addSymbolReference(std::vector<SymbolReference *> *references, char *symbol, unsigned char *addr, int len)
{
    SymbolReference *reference = (SymbolReference *)malloc(sizeof(SymbolReference));
    memset(reference, 0, sizeof(SymbolReference));
    reference->symbol = symbol;
    reference->addr = addr;
    reference->len = len;
    references->push_back(reference);
}

char saveExeFileHex(ObjectFile *object, char *filename)
{
    std::vector<CodeBlock *> code_blocks;
    std::vector<SymbolReference *> references;
    CodeFragment *fragment;

    for (ObjectSegment *object_segment : object->segments)
    {
        CodeBlock *code_block = NULL;

        for (ObjectNode *node : object_segment->nodes)
        {
            switch (node->type)
            {
            case OBJECT_NODE_OPCODE:
                if (!code_block)
                {
                    printf("warning: no address defined for segment '%s'\n", object_segment->name);
                    code_block = addCodeBlock(&code_blocks, node->d.addr.value);
                }

                fragment = addCodeToBlock(code_block, node->d.opcode.size, node->d.opcode.bytes);

                if (node->d.opcode.value)
                {
                    addSymbolReference(&references, node->d.opcode.value, 
                        &fragment->code[(int)(node->d.opcode.byte)], node->d.opcode.len);
                } 

                break;

            case OBJECT_NODE_SYMBOL:
                if (!code_block)
                {
                    printf("warning: no address defined for segment '%s'\n", object_segment->name);
                    code_block = addCodeBlock(&code_blocks, node->d.addr.value);
                }

                if (!node->d.symbol.constant)
                {
                    node->d.symbol.constant = true;
                    node->d.symbol.value = (char *)malloc(12);
                    sprintf(node->d.symbol.value, "%d", code_block->end);
                }

                break;

            case OBJECT_NODE_ADDRESS:
                code_block = addCodeBlock(&code_blocks, node->d.addr.value);
                break;

            case OBJECT_NODE_DATA:
                if (!code_block)
                {
                    printf("warning: no address defined for segment '%s'\n", object_segment->name);
                    code_block = addCodeBlock(&code_blocks, node->d.addr.value);
                }

                for (int iArg = 0; iArg < node->d.data.count; iArg++)
                {
                    char *data = node->d.data.value[iArg];

                    if (isNumber(data))
                    {
                        unsigned char *buffer = (unsigned char *)malloc(node->d.data.len);
                        int value = strtol(data, NULL, 0);
                        for (int iByte = 0; iByte < node->d.data.len; iByte++)
                        {
                            buffer[iByte] = value & 0xFF;
                            value = value >> 8;
                        }
                        addCodeToBlock(code_block, node->d.data.len, buffer);
                    }

                    else if (data[0] == '"')
                    {
                        unsigned char *buffer = (unsigned char *)malloc(node->d.data.len * (strlen(data) - 1));
                        memset(buffer, 0, node->d.data.len * (strlen(data) - 1));

                        for (int iByte = 0; iByte < (int)strlen(data); iByte++)
                            buffer[iByte * node->d.data.len] = data[iByte + 1];

                        addCodeToBlock(code_block, node->d.data.len * (strlen(data) - 1), buffer);
                    }

                    else if (isValidLabelName(data))
                    {
                        unsigned char *buffer = (unsigned char *)malloc(node->d.data.len);
                        fragment = addCodeToBlock(code_block, node->d.data.len, buffer);
                        addSymbolReference(&references, data, buffer, node->d.data.len);
                    }

                    else
                    {
                        fprintf(stderr, "unrecognized data type");
                    }
                }

                break;
            }
        }
    }

    // verifica se ocorreu alguma sobreposição
    int iCodeBlock = 0;
    for (CodeBlock *code_block : code_blocks)
    {
        if (code_block->start < 0 || code_block->start > 0xFFFF)
        {
            fprintf(stderr, "code block #%d start address is invalid: %X\n", iCodeBlock, code_block->start);
            return false;
        }

        if (code_block->end > 0x10000)
        {
            fprintf(stderr, "code block #%d end address is invalid: %X\n", iCodeBlock, code_block->end);
            return false;
        }

        for (CodeBlock *other_block : code_blocks)
        {
            if (code_block != other_block)
            {
                if (!(code_block->end < other_block->start || code_block->start > other_block->end))
                {
                    fprintf(stderr, "code block #%d overlaps the next segment\n", iCodeBlock);
                    return false;
                }
            }
        }

        iCodeBlock++;
    }

    // fixa as referências simbólicas
    char error_found = false;
    for (SymbolReference *reference : references)
    {
        ObjectNode *symbolNode = findSymbolByName(object, reference->symbol);

        if (symbolNode == NULL)
        {
            error_found = true;
            fprintf(stderr, "symbol '%s' is undefined\n", reference->symbol);
        }
        else
        {
            int value = strtol(symbolNode->d.symbol.value, NULL, 0);
            for (int iByte = 0; iByte < reference->len; iByte++)
            {
                reference->addr[iByte] = value & 0xFF;
                value = value >> 8;
            }
        }
    }

    if (error_found)
        return false;

    // cria o arquivo de saída
    FILE *fp = fopen(filename, "w");
    if (!fp)
    {
        fprintf(stderr, "failed opening '%s' (OS error %d)\n", filename, errno);
        return false;
    }

    for (CodeBlock *code_block : code_blocks)
    {
        int iByte = 0;        
        for (CodeFragment *fragment : code_block->fragments)
        {
            for (int iByteFragment = 0; iByteFragment < fragment->len; iByteFragment++)
            {
                if ((iByte % 16) == 0)
                {
                    if (iByte > 0)
                        fprintf(fp, "\n");
                    fprintf(fp, "[%4.4X] ", code_block->start + iByte);
                }

                fprintf(fp, "%2.2X ", fragment->code[iByteFragment]);
                iByte++;
            }
        }

        if ((iByte % 16) != 0)
            fprintf(fp, "\n");
    }

    fclose(fp);

    return true;
}

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <strings.h>

#include "util.h"
#include "isa.h"
#include "parser.h"

#define RESULT_EMPTY            -1
#define RESULT_INSTRUCTION      -2
#define RESULT_LABEL            -3
#define RESULT_META             -4
#define RESULT_UNEXPECTED_CHAR  -5

#define STATE_WAIT_START        1
#define STATE_STATMENT          2
#define STATE_EXPECT_END        3
#define STATE_WAIT_ARGUMENT     4
#define STATE_ARGUMENT          5
#define STATE_ARGUMENT_STRING   6
#define STATE_WAIT_SEPARATOR    7

#define DECODE_OK                           0
#define DECODE_UNKNOWN_STATEMENT            1
#define DECODE_INCORRECT_NUMBER_ARGUMENTS   2
#define DECODE_UNKNOWN_INSTRUCTION          3
#define DECODE_INCORRECT_ARGUMENT           4
#define DECODE_INVALID_SYMBOL_NAME          5
#define DECODE_DUPLICATE_SYMBOL             6

const static char *DEFAULT_SEGMENT_NAME = "text";

int parsingStateMachine(char *line, char **instruction, std::vector<char *> *args)
{
    int iChar = 0;
    int state = STATE_WAIT_START;
    int type = 0;

    *instruction = NULL;

    while (true)
    {
        if (state == STATE_WAIT_START)
        {
            if (line[iChar] == '\0' || line[iChar] == ';')
            {
                line[iChar] = '\0';
                return RESULT_EMPTY;
            }

            if (line[iChar] == '.')
            {
                type = RESULT_META;
                state = STATE_STATMENT;
                *instruction = &line[iChar++];
            }

            else if (!isWhitespace(line[iChar]))
            {
                state = STATE_STATMENT;
                *instruction = &line[iChar++];
            }

            else
            {
                iChar++;
            }
        }

        else if (state == STATE_STATMENT)
        {
            if (line[iChar] == '\0' || line[iChar] == ';')
            {
                line[iChar] = '\0';
                return type == 0 ? RESULT_INSTRUCTION : type;
            }

            if (line[iChar] == ':')
            {
                type = RESULT_LABEL;
                line[iChar++] = '\0';
                state = STATE_EXPECT_END;
            }
            
            else if (isWhitespace(line[iChar]))
            {
                if (type == 0)
                    type = RESULT_INSTRUCTION;
                line[iChar++] = '\0';
                state = STATE_WAIT_ARGUMENT;
            }

            else
            {
                iChar++;
            }
        }

        else if (state == STATE_EXPECT_END)
        {
            if (line[iChar] == '\0' || line[iChar] == ';')
            {
                line[iChar] = '\0';
                return type;
            }

            if (!isWhitespace(line[iChar]))
                return RESULT_UNEXPECTED_CHAR;

            iChar++;
        }

        else if (state == STATE_WAIT_ARGUMENT)
        {
            if (line[iChar] == '\0' || line[iChar] == ';')
            {
                line[iChar] = '\0';
                return type;
            }

            if (line[iChar] == '"' || !isWhitespace(line[iChar]))
            {
                args->push_back(&line[iChar]);
                state = line[iChar] == '"' ? STATE_ARGUMENT_STRING : STATE_ARGUMENT;
            }

            iChar++;
        }

        else if (state == STATE_ARGUMENT)
        {
            if (line[iChar] == '\0' || line[iChar] == ';')
            {
                line[iChar] = '\0';
                return type;
            }

            if (line[iChar] == ',')
            {
                line[iChar++] = '\0';
                state = STATE_WAIT_ARGUMENT;
            }

            else if (isWhitespace(line[iChar]))
            {
                line[iChar++] = '\0';
                state = STATE_WAIT_SEPARATOR;
            }

            else
            {
                iChar++;
            }
        }

        else if (state == STATE_ARGUMENT_STRING)
        {
            if (line[iChar] == '\0')
                return type;

            if (line[iChar] == '"')
            {
                state = STATE_WAIT_SEPARATOR;
                line[iChar++] = '\0';
            }

            else
            {
                iChar++;
            }
        }

        else if (state == STATE_WAIT_SEPARATOR)
        {
            if (line[iChar] == '\0' || line[iChar] == ';')
            {
                line[iChar] = '\0';
                return type;
            }

            if (line[iChar] == ',')
            {
                state = STATE_WAIT_ARGUMENT;
                iChar++;
            }

            else if (!isWhitespace(line[iChar]))
            {
                return RESULT_UNEXPECTED_CHAR;
            }

            else
            {
                iChar++;
            }
        }
    }
}

char *deriveTypesFromArgs(std::vector<char *> *args)
{
    std::vector<char *> types;

    for (char *arg : *args)
    {
        char type[8] = "";
        const char *registerName = normalizeRegisterName(arg, true);
        
        if (registerName)
            strcpy(type, getRegisterType(registerName));

        if (!type[0])
        {
            if (isValidLabelName(arg) || isNumber(arg))
            {
                int value = strtol(arg, NULL, 0);
                if (value >= 0 && value < 16)
                    strcpy(type, "m4");
                else if (value >= -128 && value < 256)
                    strcpy(type, "m8");
                else
                    strcpy(type, "m16");
            }
        }

        if (!type[0])
        {
            if (arg[0] == '[' && arg[strlen(arg) - 1] == ']')
            {
                strcpy(type, "[");
                arg[strlen(arg) - 1] = '\0';

                registerName = normalizeRegisterName(&arg[1], true);
                if (registerName)
                {
                    strcpy(type, getRegisterType(registerName));
                }
                else
                {
                    int value = strtol(arg, NULL, 0);
                    if (value >= 0 && value < 16)
                        strcpy(type, "m4");
                    else if (value >= -128 && value < 256)
                        strcpy(type, "m8");
                    else
                        strcpy(type, "m16");
                }
            }
        }

        if (!type[0])
            return NULL;
 
        types.push_back(strdup(type));
    }

    // calcula o tamanho da lista de argumentos
    int len = 0;
    for (char *type : types)
    {
        if (len > 0)
            len++;
        len += strlen(type);
    }

    char *result = (char *)malloc(len + 1);
    result[0] = '\0';
    int index = 0;
    for (char *type : types)
    {
        if (index > 0)
            result[index++] = '_';
        strcpy(&result[index], type);
        index += strlen(type);
    }

    return result;
}

ObjectSegment *createObjectSegment(const char *name, const char *source_file)
{
    ObjectSegment *segment = (ObjectSegment *)malloc(sizeof(ObjectSegment));
    memset(segment, 0, sizeof(ObjectSegment));
    segment->name = strdup(name);
    segment->source_name = strdup(source_file);
    return segment;
}

int decodeInstruction(char *instruction, std::vector<char *> *args, ObjectFile *objectFile)
{
    char *types = deriveTypesFromArgs(args);
    const Instruction *ins = findInstructionByName(instruction, types);

    if (!ins)
        return DECODE_UNKNOWN_INSTRUCTION;    

    ObjectNode *node = (ObjectNode *)malloc(sizeof(ObjectNode));
    memset(node, 0, sizeof(ObjectNode));

    unsigned char opcode[16];
    memset(opcode, 0, sizeof(opcode));

    node->type = OBJECT_NODE_OPCODE;    

    char *symbolName = NULL;
    char symbolStart;
    char symbolLen;

    node->d.opcode.size = ins->builder(opcode, ins->options, args, &symbolName, &symbolStart, &symbolLen);

    node->d.opcode.bytes = (unsigned char *)malloc(node->d.opcode.size);
    memcpy(node->d.opcode.bytes, opcode, node->d.opcode.size);

    if (symbolName)
    {
        node->d.opcode.value = strdup(symbolName);
        node->d.opcode.byte = symbolStart;
        node->d.opcode.len = symbolLen;
    }

    if (objectFile->active_segment == NULL)
    {
        objectFile->active_segment = createObjectSegment(DEFAULT_SEGMENT_NAME, objectFile->source_name);
        objectFile->segments.push_back(objectFile->active_segment);
    }

    objectFile->active_segment->nodes.push_back(node);

    return DECODE_OK;
}

int decodeLabel(char *label, std::vector<char *> *args, ObjectFile *objectFile)
{
    if (args->size() != 0)
        return DECODE_INCORRECT_NUMBER_ARGUMENTS;

    if (!isValidLabelName(label))
        return DECODE_INVALID_SYMBOL_NAME;

    if (findSymbolByName(objectFile, label) != NULL)
        return DECODE_DUPLICATE_SYMBOL;

    ObjectNode *node = (ObjectNode *)malloc(sizeof(ObjectNode));
    memset(node, 0, sizeof(ObjectNode));

    node->type = OBJECT_NODE_SYMBOL;

    node->d.symbol.name = strdup(label);
    node->d.symbol.constant = false;

    if (objectFile->active_segment == NULL)
    {
        objectFile->active_segment = createObjectSegment(DEFAULT_SEGMENT_NAME, objectFile->source_name);
        objectFile->segments.push_back(objectFile->active_segment);
    }

    objectFile->active_segment->nodes.push_back(node);
    objectFile->symbols.push_back(node);

    return DECODE_OK;
}

int decodeMeta(char *meta, std::vector<char *> *args, ObjectFile *objectFile)
{
    if (strcmp(meta, ".org") == 0)
    {
        if (args->size() != 1)
            return DECODE_INCORRECT_NUMBER_ARGUMENTS;

        if (!isNumber(args->at(0)))
            return DECODE_INCORRECT_ARGUMENT;

        ObjectNode *node = (ObjectNode *)malloc(sizeof(ObjectNode));
        memset(node, 0, sizeof(ObjectNode));

        node->type = OBJECT_NODE_ADDRESS;
        node->d.addr.value = strtol(args->at(0), NULL, 0);

        if (objectFile->active_segment == NULL)
        {
            objectFile->active_segment = createObjectSegment(DEFAULT_SEGMENT_NAME, objectFile->source_name);
            objectFile->segments.push_back(objectFile->active_segment);
        }

        objectFile->active_segment->nodes.push_back(node);
    }

    else if (strcmp(meta, ".db") == 0 || strcmp(meta, ".dw") == 0)
    {
        if (args->size() == 0)
            return DECODE_INCORRECT_NUMBER_ARGUMENTS;

        ObjectNode *node = (ObjectNode *)malloc(sizeof(ObjectNode));
        memset(node, 0, sizeof(ObjectNode));

        node->type = OBJECT_NODE_DATA;
        node->d.data.len = strcmp(meta, ".db") == 0 ? 1 : 2;
        node->d.data.count = args->size();
        node->d.data.value = (char **)malloc(args->size() * sizeof(char *));

        int iArg = 0;
        for (char *arg : *args)
            node->d.data.value[iArg++] = strdup(arg);

        if (objectFile->active_segment == NULL)
        {
            objectFile->active_segment = createObjectSegment(DEFAULT_SEGMENT_NAME, objectFile->source_name);
            objectFile->segments.push_back(objectFile->active_segment);
        }

        objectFile->active_segment->nodes.push_back(node);
    }

    else if (strcmp(meta, ".equ") == 0)
    {
        if (args->size() < 2)
            return DECODE_INCORRECT_NUMBER_ARGUMENTS;

        // TODO
    }

    else if (strcmp(meta, ".segment") == 0)
    {
        if (args->size() != 1)
            return DECODE_INCORRECT_NUMBER_ARGUMENTS;

        objectFile->active_segment = findSegmentByName(objectFile, args->at(0));

        if (objectFile->active_segment == NULL)
        {
            objectFile->active_segment = createObjectSegment(args->at(0), objectFile->source_name);
            objectFile->segments.push_back(objectFile->active_segment);
        }
    }

    else
    {
        return DECODE_UNKNOWN_STATEMENT;
    }

    return DECODE_OK;
}

ObjectFile *parseAsmFile(char *sourceFilename, FILE *errorFile)
{
    FILE *fsourceFile = fopen(sourceFilename, "r");

    if (fsourceFile == NULL)
    {
        fprintf(errorFile, "can't open '%s' (OS %d)\n", sourceFilename, errno);
        return NULL;
    }

    int lineno = 0;
    char error_found = false;

    ObjectFile *objectFile = (ObjectFile *)malloc(sizeof(ObjectFile));
    memset(objectFile, 0, sizeof(ObjectFile));
    objectFile->source_name = strdup(sourceFilename);

    while (true)
    {
        char buffer[1024];
        fgets(buffer, sizeof(buffer), fsourceFile);
        if (feof(fsourceFile))
            break;

        lineno++;

        buffer[sizeof(buffer) - 1] = '\0';

        char *instruction;
        std::vector<char *> args;
        int state = parsingStateMachine(buffer, &instruction, &args);

        int decodeResult = DECODE_OK;

        if (state == RESULT_INSTRUCTION)
            decodeResult = decodeInstruction(instruction, &args, objectFile);

        else if (state == RESULT_LABEL)
            decodeResult = decodeLabel(instruction, &args, objectFile);

        else if (state == RESULT_META)
            decodeResult = decodeMeta(instruction, &args, objectFile);

        else if (state == RESULT_UNEXPECTED_CHAR)
            fprintf(errorFile, "[%s:%d] unexpected char\n", sourceFilename, lineno);

        if (decodeResult != DECODE_OK)
        {
            error_found = true;

            switch (decodeResult)
            {
            case DECODE_UNKNOWN_STATEMENT:
                fprintf(errorFile, "[%s:%d] unknown statement\n", sourceFilename, lineno);
                break;

            case DECODE_INCORRECT_NUMBER_ARGUMENTS:
                fprintf(errorFile, "[%s:%d] incorrect number of arguments\n", sourceFilename, lineno);
                break;

            case DECODE_UNKNOWN_INSTRUCTION:
                fprintf(errorFile, "[%s:%d] unknown instruction\n", sourceFilename, lineno);
                break;

            case DECODE_INCORRECT_ARGUMENT:
                fprintf(errorFile, "[%s:%d] incorrect argument\n", sourceFilename, lineno);
                break;

            case DECODE_INVALID_SYMBOL_NAME:
                fprintf(errorFile, "[%s:%d] invalid symbol name\n", sourceFilename, lineno);
                break;

            case DECODE_DUPLICATE_SYMBOL:
                fprintf(errorFile, "[%s:%d] duplicate symbol\n", sourceFilename, lineno);
                break;

            default:
                fprintf(errorFile, "[%s:%d] undefined error code (%d)\n", sourceFilename, lineno, decodeResult);
                break;
            }
        }
    }

    fclose(fsourceFile);

    // valida a exist�ncia dos s�mbolos locais
    if (!error_found)
    {
        for (ObjectSegment *object_segment : objectFile->segments)
        {
            for (ObjectNode *node : object_segment->nodes)
            {
                switch (node->type)
                {
                case OBJECT_NODE_OPCODE:
                    if (node->d.opcode.value)
                    {
                        if (node->d.opcode.value[0] == '_' && findSymbolByName(objectFile, node->d.opcode.value) == NULL)
                        {
                            fprintf(errorFile, "undefined symbol '%s'\n", node->d.opcode.value);
                            error_found = true;
                        }
                    } 
                    break;

                case OBJECT_NODE_DATA:
                    for (int iArg = 0; iArg < node->d.data.count; iArg++)
                    {
                        char *data = node->d.data.value[iArg];
                        if (isValidLabelName(data))
                        {
                            if (data[0] == '_' && findSymbolByName(objectFile, data) == NULL)
                            {
                                fprintf(errorFile, "undefined symbol '%s'\n", data);
                                error_found = true;
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    return error_found ? NULL : objectFile;
}

ObjectNode *findSymbolByName(const ObjectFile *objectFile, const char *symbol)
{
    for (ObjectNode *objectNode : objectFile->symbols)
        if (strcasecmp(objectNode->d.symbol.name, symbol) == 0)
            return objectNode;
    return NULL;
}

ObjectSegment *findSegmentByName(const ObjectFile *objectFile, const char *segment_name)
{
    for (ObjectSegment *objectSegment : objectFile->segments)
        if (strcasecmp(objectSegment->name, segment_name) == 0)
            return objectSegment;
    return NULL;
}

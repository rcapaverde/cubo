#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "context.h"

#define UCC_VERSION "2.00.00"

#define OUTPUT_LOGICSIM 1
#define OUTPUT_HEX      2
#define OUTPUT_BINARY   3


CodeStep primitives[] = {
    {"NOP",             0xFF, 0xFF},

    {"WR_MEMD_MAR",     0x0D, 0xCF},
    {"WR_MEMD_SP",      0x0C, 0xCF},
    {"WR_PCL",          0x0B, 0xCF},
    {"WR_PCH",          0x0A, 0xCF},
    {"WR_MAR",          0x09, 0xCF},
    {"WR_IOD_MAR",      0x08, 0xCF},
    {"WR_B",            0x07, 0xCF},
    {"WR_C",            0x06, 0xCF},
    {"WR_D",            0x05, 0xCF},
    {"WR_L",            0x04, 0xCF},
    {"WR_H",            0x03, 0xCF},
    {"WR_SP",           0x02, 0xCF},
    {"WR_PSW",          0x01, 0xCF},
    {"WR_ACC",          0x00, 0xCF},

    {"CONST_INTL",      0x30, 0x30},
    {"CONST_INTH",      0x20, 0x30},
    {"CONST_RSTL",      0x10, 0x30},
    {"CONST_RSTH",      0x00, 0x30},

    {"RD_ZERO",         0x5F, 0xDF},
    {"RD_MEMD_PC",      0x4F, 0xDF},
    {"RD_MEMD_CONST",   0x4E, 0xDF},
    {"RD_MEMD_MAR",     0x4D, 0xDF},
    {"RD_MEMD_SP",      0x4C, 0xDF},
    {"RD_PCL",          0x4B, 0xDF},
    {"RD_PCH",          0x4A, 0xDF},
    {"RD_MAR",          0x49, 0xDF},
    {"RD_IOD_MAR",      0x48, 0xDF},
    {"RD_B",            0x47, 0xDF},
    {"RD_C",            0x46, 0xDF},
    {"RD_D",            0x45, 0xDF},
    {"RD_L",            0x44, 0xDF},
    {"RD_H",            0x43, 0xDF},
    {"RD_SP",           0x42, 0xDF},
    {"RD_PSW",          0x41, 0xDF},
    {"RD_ACC",          0x40, 0xDF},

    {"SET_ADD",         0x3F, 0xFF},
    {"SET_SUB",         0x3E, 0xFF},
    {"SET_CMP",         0x3D, 0xFF},
    {"SET_AND",         0x3C, 0xFF},
    {"SET_OR",          0x3B, 0xFF},
    {"SET_XOR",         0x3A, 0xFF},
    {"SET_NOT",         0x39, 0xFF},
    {"SET_SHR",         0x38, 0xFF},
    {"SET_SHL",         0x37, 0xFF},
    {"SET_ROR",         0x36, 0xFF},
    {"SET_ROL",         0x35, 0xFF},
    {"SET_SX",          0x34, 0xFF},
    {"SET_SETCF",       0x33, 0xFF},
    {"SET_CLRCF",       0x32, 0xFF},
    {"SET_SETIF",       0x31, 0xFF},
    {"SET_CLRIF",       0x30, 0xFF},

    {"ULA",             0x20, 0x20},

    {"PC_INC",          0x86, 0xC7},
    {"MAR_INC",         0x85, 0xC7},
    {"MAR_DEC",         0x84, 0xC7},
    {"SP_INC",          0x83, 0xC7},
    {"SP_DEC",          0x82, 0xC7},
    {"CLEAR_INTR",      0x81, 0xC7},

    {"EXIT",            0xB0, 0x38},
    {"CONDITION",       0xA8, 0x38},
    {"EXTEND",          0xA0, 0x38},
    {"FINISH",          0x98, 0x38},
    {"NOT_CONDITION",   0x90, 0x38},
    {"HALT",            0x88, 0x38},

    {"COND_Z",          0x0E, 0xF0},
    {"COND_N",          0x0D, 0xF0},
    {"COND_C",          0x0B, 0xF0},
    {"COND_O",          0x07, 0xF0},

    {NULL,              0x00, 0x00},
};

static CodeStep *findStepByName(char *name)
{
    for (int iStep = 0; primitives[iStep].name; iStep++)
        if (stricmp(primitives[iStep].name, name) == 0)
            return &primitives[iStep];
    return NULL;
}

static char isWhitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static void parseArguments(char *line, int *argc, char **argv)
{
    int index = 0;
    char emToken = false;
    char emStr = false;
    int len = strlen(line);

    *argc = 0;

    while (index < len)
    {
        if (emToken)
        {
            if (emStr)
            {
                if (line[index] == '"')
                {
                    line[index] = '\0';
                    emToken = false;
                    emStr = false;
                }
            }
            else
            {
                if (isWhitespace(line[index]))
                {
                    line[index] = '\0';
                    emToken = false;
                }
            }
        }
        else
        {
            if (line[index] == ';')
                return;

            if (line[index] == '"' || !isWhitespace(line[index]))
            {
                emStr = line[index] == '"';
                if (emStr)
                    index++;
                    
                argv[(*argc)++] = &line[index];
                emToken = true;
            }
        }

        index++;
    }
}

char compile(CompileContext *context, char *sourceFilename, FILE *errorFile)
{
    FILE *fsourceFile = fopen(sourceFilename, "r");

    if (fsourceFile == NULL)
    {
        fprintf(errorFile, "can't open '%s' (OS %d)\n", sourceFilename, errno);
        return false;
    }

    int lineno = 0;
    char errorFound = false;
    CompileMacro *macro = NULL;

    while (true)
    {
        char buffer[1024];
        fgets(buffer, sizeof(buffer), fsourceFile);
        if (feof(fsourceFile))
            break;
        lineno++;

        buffer[sizeof(buffer) - 1] = '\0';

        int argc;
        char *argv[sizeof(buffer) / 2];      
        parseArguments(buffer, &argc, argv);

        if (argc)
        {
            if (stricmp(argv[0], ".inc") == 0)
            {
                if (argc != 2)
                {
                    fprintf(errorFile, "[file %s, line %d] incorrect number of arguments\n", sourceFilename, lineno);
                    errorFound = true;
                }
                else
                {
                    if (!compile(context, argv[1], errorFile))
                        errorFound = true;
                }
            }

            else
            {
                if (argv[0][strlen(argv[0]) - 1] == ':')
                {
                    if (argc != 1)
                    {
                        fprintf(errorFile, "[file %s, line %d] macro definitions cannot have arguments\n", sourceFilename, lineno);
                        errorFound = true;
                    }
                    else
                    {
                        argv[0][strlen(argv[0]) - 1] = '\0';
                        if (strlen(argv[0]) == 0)
                        {
                            fprintf(errorFile, "[file %s, line %d] invalid macro name\n", sourceFilename, lineno);
                            errorFound = true;
                        }
                        else
                        {
                            CompileMacro *redefined = compile_getMacro(context, argv[0]);
                            if (redefined)
                            {
                                fprintf(errorFile, "[file %s, line %d] redefined macro '%s' (seen in %s:%d)\n", 
                                    sourceFilename, lineno, argv[0], redefined->filename, redefined->lineno);
                                errorFound = true;
                            }
                            else
                            {
                                CodeStep *step = findStepByName(argv[0]);
                                if (step)
                                {
                                    fprintf(errorFile, "[file %s, line %d] forbiden macro name '%s'\n", 
                                        sourceFilename, lineno, argv[0]);
                                    errorFound = true;
                                }
                                else
                                {
                                    macro = compile_appendMacro(context, argv[0], sourceFilename, lineno);
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (!macro)
                    {
                        fprintf(errorFile, "[file %s, line %d] no macro defined; unexpected token\n", sourceFilename, lineno);
                        errorFound = true;
                    }
                    else
                    {
                        for (int iToken = 0; iToken < argc; iToken++)
                        {
                            char *token = argv[iToken];

                            CodeStep *stepPrimitive = findStepByName(token);
                            CompileMacro *stepMacro = NULL;

                            if (!stepPrimitive)
                            {
                                stepMacro = compile_getMacro(context, token);

                                if (!stepMacro)
                                {
                                    fprintf(errorFile, "[file %s, line %d] unknown token '%s'\n", sourceFilename, lineno, token);
                                    errorFound = true;
                                    break;
                                }
                            }

                            int step = 0;
                            while (true)
                            {
                                if (macro->count >= MAX_STEPS)
                                {
                                    fprintf(errorFile, "[file %s, line %d] macro is too long\n", sourceFilename, lineno);
                                    errorFound = true;
                                    break;
                                }

                                unsigned char mask = 0;
                                unsigned char signals = 0;

                                if (stepPrimitive)
                                {
                                    mask = stepPrimitive->mask;
                                    signals = stepPrimitive->signal;
                                }
                                else
                                {
                                    mask = stepMacro->steps[step].mask;
                                    signals = stepMacro->steps[step].signal;
                                }

                                if ((macro->steps[macro->count].mask & mask) != 0)
                                {
                                    fprintf(errorFile, "[file %s, line %d] signal conflict\n", sourceFilename, lineno);
                                    errorFound = true;
                                    break;
                                }
                                else
                                {
                                    macro->steps[macro->count].mask |= mask;
                                    macro->steps[macro->count].signal |= signals;
                                }

                                if (!stepMacro || step >= stepMacro->count - 1)
                                    break;

                                macro->count++;
                                step++;
                            }
                        }

                        macro->count++;
                    }
                }
            }
        }
    }

    return !errorFound;
}

int main(int argc, char *argv[])
{
    printf("cubo ucode compiler %s\n", UCC_VERSION);

    char *sourceFile = NULL;
    int outputFormat = OUTPUT_LOGICSIM;
    char createMap = false;

    for (int iArg = 1; iArg < argc; iArg++)
    {
        if (strcmp(argv[iArg], "-ls") == 0)
            outputFormat = OUTPUT_LOGICSIM;

        else if (strcmp(argv[iArg], "-hex") == 0)
            outputFormat = OUTPUT_HEX;

        else if (strcmp(argv[iArg], "-bin") == 0)
            outputFormat = OUTPUT_BINARY;

        else if (strcmp(argv[iArg], "-map") == 0)
            createMap = true;

        else if (argv[iArg][0] == '-')
        {
            printf("unknown parameter: %s\n", argv[iArg]);
            return 1;
        }

        else if (sourceFile == NULL)
            sourceFile = argv[iArg];
    }

    if (!sourceFile)
    {
        printf("missing source file name\n");
        return 1;
    }

    CompileContext context;
    memset(&context, 0, sizeof(CompileContext));

    if (!compile(&context, sourceFile, stderr))
    {
        printf("failed; see messages above\n");
        return 1;
    }

    char romName[256];
    sprintf(romName, "%s.rom", sourceFile);
    FILE *fp_rom = fopen(romName, "wb");
    if (fp_rom == NULL)
    {
        printf("failed creating '%s' (OS %d)\n", romName, errno);
        return 1;
    }

    unsigned char signals[MAX_INSTRUCTIONS][MAX_STEPS];
    memset(signals, 0x00, MAX_INSTRUCTIONS * MAX_STEPS);

    CompileMacro *defaultCode = compile_getMacro(&context, "DEFAULT");

    FILE *fp_map = NULL;
    if (createMap)
    {
        char mapName[256];
        sprintf(mapName, "%s.map", sourceFile);
        fp_map = fopen(mapName, "w");
        if (fp_map == NULL)
        {
            printf("failed creating '%s' (OS %d)\n", mapName, errno);
            return 1;
        }
    }

    for (int iInstruction = 0; iInstruction < MAX_INSTRUCTIONS; iInstruction++)
    {
        char instructionCode[8];
        sprintf(instructionCode, "%2.2X", iInstruction);
        CompileMacro *instr = compile_getMacro(&context, instructionCode);

        if (instr != NULL)
            if (fp_map)
                fprintf(fp_map, "[%s] %d steps (%s:%d)\n", instructionCode, instr->count, instr->filename, instr->lineno);

        if (instr == NULL)
            instr = defaultCode;

        if (instr != NULL)
        {
            for (int iStep = 0; iStep < instr->count; iStep++)
                signals[iInstruction][iStep] = instr->steps[iStep].signal;
        }
    }

    if (outputFormat == OUTPUT_BINARY)
    {
        for (int iInstruction = 0; iInstruction < MAX_INSTRUCTIONS; iInstruction++)
            for (int iStep = 0; iStep < MAX_STEPS; iStep++)
                fwrite(&signals[iInstruction][iStep], 1, 1, fp_rom);
    }

    else if (outputFormat == OUTPUT_HEX)
    {
        for (int iInstruction = 0; iInstruction < MAX_INSTRUCTIONS; iInstruction++)
            for (int iStep = 0; iStep < MAX_STEPS; iStep++)
                fprintf(fp_rom, "%2.2X%s", signals[iInstruction][iStep], iStep == MAX_STEPS - 1 ? "\r\n" : " ");
    }

    else if (outputFormat == OUTPUT_LOGICSIM)
    {
        int nPrints = 0;

        unsigned char prevByte = 0;
        int counter = 0;
        fprintf(fp_rom, "v2.0 raw\n");

        for (int iInstruction = 0; iInstruction < MAX_INSTRUCTIONS; iInstruction++)
        {
            for (int iStep = 0; iStep < MAX_STEPS; iStep++)
            {
                if (prevByte == signals[iInstruction][iStep])
                {
                    counter++;
                }
                else
                {
                    if (counter == 1)
                        fprintf(fp_rom, "%2.2X ", prevByte);
                    else if (counter > 1)
                        fprintf(fp_rom, "%d*%2.2X ", counter, prevByte);

                    nPrints++;
                    prevByte = signals[iInstruction][iStep];
                    counter = 1;

                    if (nPrints >= 16)
                    {
                        fprintf(fp_rom, "\n");
                        nPrints = 0;
                    }
                }
            }
        }

        if (counter == 1)
            fprintf(fp_rom, "%2.2X\n", prevByte);
        else if (counter > 1)
            fprintf(fp_rom, "%d*%2.2X\n", counter, prevByte);
    }

    fclose(fp_rom);

    return 0;
}

#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <strings.h>

#include "util.h"
#include "isa.h"

#define INS_CONTROL         (0 << 4)
#define INS_TRANSFER8       (1 << 4)
#define INS_TRANSFER16      (2 << 4)
#define INS_TRANSFER8_IND   (3 << 4)
#define INS_TRANSFER16_IND  (4 << 4)
#define INS_ULA             (5 << 4)
#define INS_ULA_IND         (6 << 4)
#define INS_ULA_SINGLE      (7 << 4)
#define INS_IN_OUT          (8 << 4)

#define REG_ZERO            0
#define REG_A               1 
#define REG_B               2 
#define REG_C               3 
#define REG_D               4 
#define REG_E               5 
#define REG_H               6 
#define REG_L               7 
#define REG_MEMD_FROM_PC    8
#define REG_MEMD_FROM_MAR   9 
#define REG_MEMD_FROM_SP    10
#define REG_MEMD_FROM_CONST 11
#define REG_IOD_FROM_MAR    12
#define REG_PSW             14
#define REG_TEMP            15

#define REG_BC              1
#define REG_DE              2
#define REG_HL              3
#define REG_PC              12
#define REG_SP              13
#define REG_MAR             14

#define INS_P1  0
#define INS_P2  4
#define INS_P3  0

#define CONTROL_FINISH  1
#define CONTROL_HASH    2

static void copy_immediate8(unsigned char *opcode, int start, char *str, char **symbolName, char *symbolStart, char *symbolLen)
{
    if (isNumber(str))
    {
        opcode[start] = strtol(str, NULL, 0);
    }
    else
    {
        *symbolName = str;
        *symbolStart = start;
        *symbolLen = 1;
    }
}

static void copy_immediate16(unsigned char *opcode, int start, char *str, char **symbolName, char *symbolStart, char *symbolLen)
{
    if (isNumber(str))
    {
        unsigned short value = strtol(str, NULL, 0);
        opcode[start] = value & 0xFF;
        opcode[start + 1] = (value >> 8) & 0xFF;
    }
    else
    {
        *symbolName = str;
        *symbolStart = start;
        *symbolLen = 2;
    }
}

static int Opcode_Control(unsigned char *opcode, int options, 
    std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = (unsigned char)(INS_CONTROL | (options << INS_P3));
    return 2;
}

static int Opcode_Move(unsigned char *opcode, int options, 
    std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = INS_TRANSFER8;
    opcode[1] = (unsigned char)((getRegisterIndex(args->at(1)) << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1));
    return 2;
}

static int Opcode_Load8(unsigned char *opcode, int options, 
    std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = INS_TRANSFER8;
    opcode[1] = (unsigned char)((REG_MEMD_FROM_PC << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1));
    copy_immediate8(opcode, 2, args->at(1), symbolName, symbolStart, symbolLen);
    return 3;
}

static int Opcode_Load16(unsigned char *opcode, int options, 
    std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = INS_TRANSFER8;
    opcode[1] = (unsigned char)((REG_MEMD_FROM_PC << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1));
    copy_immediate16(opcode, 2, args->at(1), symbolName, symbolStart, symbolLen);
    return 4;
}

static int Opcode_Load8_Indirect_Reg(unsigned char *opcode, int options, 
    std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = (unsigned char)(INS_TRANSFER8_IND | (REG_MEMD_FROM_MAR << INS_P3));
    opcode[1] = (unsigned char)((getRegisterIndex(args->at(1)) << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1));
    return 2;
}

static int Opcode_Load8_Indirect(unsigned char *opcode, int options, 
    std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = (unsigned char)(INS_TRANSFER8_IND | (REG_MEMD_FROM_MAR << INS_P3));
    opcode[1] = (unsigned char)((getRegisterIndex(args->at(1)) << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1));
    copy_immediate16(opcode, 2, args->at(1), symbolName, symbolStart, symbolLen);
    return 4;
}

static int Opcode_Load16_Indirect_Reg(unsigned char *opcode, int options, 
    std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = (unsigned char)(INS_TRANSFER16_IND | (REG_MEMD_FROM_MAR << INS_P3));
    opcode[1] = (unsigned char)((getRegisterIndex(args->at(1)) << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1));
    return 2;
}

static int Opcode_Load16_Indirect(unsigned char *opcode, int options, 
    std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = (unsigned char)(INS_TRANSFER16_IND | (REG_MEMD_FROM_MAR << INS_P3));
    opcode[1] = (unsigned char)((getRegisterIndex(args->at(1)) << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1));
    copy_immediate16(opcode, 2, args->at(1), symbolName, symbolStart, symbolLen);
    return 4;
}

static int Opcode_In_Out(unsigned char *opcode, int options, 
    std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = (unsigned char)(INS_IN_OUT | (REG_IOD_FROM_MAR << INS_P3));
    opcode[1] = (unsigned char)((getRegisterIndex(args->at(1)) << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1));
    return 2;
}

static int Opcode_Ula_No_Parms(unsigned char *opcode, int options, 
    std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = (unsigned char)(INS_ULA_SINGLE | (options << INS_P3));
    opcode[1] = (unsigned char)(REG_ZERO << INS_P1);
    return 2;
}

static int Opcode_Ula(unsigned char *opcode, int options, 
    std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = (unsigned char)(INS_ULA | (options << INS_P3));
    opcode[1] = (unsigned char)((getRegisterIndex(args->at(1)) << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1));
    return 2;
}

static int Opcode_Jump(unsigned char *opcode, int options, 
    std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = (unsigned char)(INS_TRANSFER16_IND | (REG_MAR << INS_P3));
    opcode[1] = (unsigned char)((REG_MEMD_FROM_PC << INS_P2) | (REG_PC << INS_P1));
    copy_immediate16(opcode, 2, args->at(0), symbolName, symbolStart, symbolLen);
    return 4;
}

const static Register REGISTERS[] = {
    {0, "zero", "reg8", true},
    {1, "a", "reg8", true},
    {2, "b", "reg8", true},
    {3, "c", "reg8", true},
    {4, "d", "reg8", true},
    {5, "e", "reg8", true},
    {6, "h", "reg8", true},
    {7, "l", "reg8", true},
    {8, "MEMD_PC", "reg8", false},
    {9, "MEMD_MAR", "reg8", false},
    {10, "MEMD_SP", "reg8", false},
    {11, "MEMD_CONST", "reg8", false},
    {12, "IOD_MAR", "reg8", false},
    {14, "psw", "reg8", true},
    {15, "TEMP", "reg8", false},
    {1, "bc", "reg16", true},
    {2, "de", "reg16", true},
    {3, "hl", "reg16", true},
    {8, "pc", "reg16", true},
    {9, "sp", "reg16", true},
    {10, "MAR", "reg16", false},
    {0, NULL, NULL, false}
};

#define ULA_ADD     0
#define ULA_SUB     1
#define ULA_CMP     2
#define ULA_AND     3
#define ULA_OR      4
#define ULA_XOR     5
#define ULA_NOT     6
#define ULA_SHR     7
#define ULA_SHL     8
#define ULA_ROR     9
#define ULA_ROL     10
#define ULA_SX      11
#define ULA_SETCF   12
#define ULA_CLRCF   13
#define ULA_SETIF   14
#define ULA_CLRIF   15

const static Instruction ISA[] =
{
    {"nop", "", Opcode_Control, CONTROL_FINISH},
    {"hlt", "", Opcode_Control, CONTROL_HASH},

    {"mov", "reg8_reg8", Opcode_Move, 0},
    {"mov", "reg16_reg16", Opcode_Move, 0},

    {"ld", "reg8_m4", Opcode_Load8, 0},
    {"ld", "reg8_m8", Opcode_Load8, 0},
    {"ld", "reg16_m4", Opcode_Load16, 0},
    {"ld", "reg16_m8", Opcode_Load16, 0},
    {"ld", "reg16_m16", Opcode_Load16, 0},

    {"ld", "reg8_[reg16", Opcode_Load8_Indirect_Reg, 0},
    {"ld", "reg8_[m4", Opcode_Load8_Indirect, 0},
    {"ld", "reg8_[m8", Opcode_Load8_Indirect, 0},
    {"ld", "reg8_[m16", Opcode_Load8_Indirect, 0},

    {"ld", "reg16_[reg16", Opcode_Load16_Indirect_Reg, 0},
    {"ld", "reg16_[m4", Opcode_Load16_Indirect, 0},
    {"ld", "reg16_[m8", Opcode_Load16_Indirect, 0},
    {"ld", "reg16_[m16", Opcode_Load16_Indirect, 0},

    {"out", "reg8_reg8", Opcode_In_Out, 0},

    {"ei", "", Opcode_Ula_No_Parms, ULA_SETIF},
    {"di", "", Opcode_Ula_No_Parms, ULA_CLRIF},
    {"clc", "", Opcode_Ula_No_Parms, ULA_CLRCF},
    {"stc", "", Opcode_Ula_No_Parms, ULA_SETCF},

    {"adc", "reg8_reg8", Opcode_Ula, ULA_ADD},

    {"jmp", "m4", Opcode_Jump, 0},
    {"jmp", "m8", Opcode_Jump, 0},
    {"jmp", "m16", Opcode_Jump, 0},

    {NULL, NULL, NULL, 0}
};


const char *normalizeRegisterName(const char *name, char user)
{
    for (int i = 0; REGISTERS[i].name ; i++)
    {
        const char *registerName = REGISTERS[i].name;
        if (strcasecmp(registerName, name) == 0 && (REGISTERS[i].user || !user))
            return registerName;
    }

    return NULL;
}

const char *getRegisterType(const char *name)
{
    for (int i = 0; REGISTERS[i].name ; i++)
        if (strcmp(REGISTERS[i].name, name) == 0)
            return REGISTERS[i].type;
    return NULL;
}

const char getRegisterIndex(const char *name)
{
    for (int i = 0; REGISTERS[i].name ; i++)
        if (strcmp(REGISTERS[i].name, name) == 0)
            return REGISTERS[i].index;
    return -1;
}

const Instruction *findInstructionByName(char *instruction, char *types)
{
    for (int i = 0; ISA[i].name; i++)
    {
        const Instruction *ins = &ISA[i];
        if (strcasecmp(ins->name, instruction) == 0 && strcasecmp(ins->signature, types) == 0)
            return ins;
    }

    return NULL;
}

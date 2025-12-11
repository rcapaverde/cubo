#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <strings.h>

#include "util.h"
#include "isa.h"

#define INS_CONTROL         (0 << 12)
#define INS_TRANSFER8       (1 << 12)
#define INS_TRANSFER16      (2 << 12)
#define INS_TRANSFER8_IND   (3 << 12)
#define INS_TRANSFER16_IND  (4 << 12)
#define INS_ULA             (5 << 12)
#define INS_ULA_SINGLE      (6 << 12)
#define INS_ULA_IND         (7 << 12)

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
#define REG_PC              8
#define REG_SP              9
#define REG_MAR             10

#define INS_P1  0
#define INS_P2  4
#define INS_P3  8

#define CONTROL_FINISH  1
#define CONTROL_HASH    2

static void Opcode_Nop(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
static void Opcode_Halt(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
static void Opcode_Load8(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen);
static void Opcode_Load16(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen);
static void Opcode_Load8_Indirect_Reg(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen);
static void Opcode_Load8_Indirect(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen);


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

const static Instruction ISA[] =
{
    {"nop", "", 2, Opcode_Nop},
    {"hlt", "", 2, Opcode_Halt},

    {"mov", "reg8_reg8", 2, Opcode_Move},
    {"mov", "reg16_reg16", 2, Opcode_Move},

    {"ld", "reg8_m4", 3, Opcode_Load8},
    {"ld", "reg8_m8", 3, Opcode_Load8},
    {"ld", "reg16_m4", 4, Opcode_Load16},
    {"ld", "reg16_m8", 4, Opcode_Load16},
    {"ld", "reg16_m16", 4, Opcode_Load16},

    {"ld", "reg8_[reg16", 2, Opcode_Load8_Indirect_Reg},
    {"ld", "reg8_[m4", 4, Opcode_Load8_Indirect},
    {"ld", "reg8_[m8", 4, Opcode_Load8_Indirect},
    {"ld", "reg8_[m16", 4, Opcode_Load8_Indirect},

    {"ld", "reg16_[reg16", 2, Opcode_Load16_Indirect_Reg},
    {"ld", "reg16_[m4", 4, Opcode_Load16_Indirect},
    {"ld", "reg16_[m8", 4, Opcode_Load16_Indirect},
    {"ld", "reg16_[m16", 4, Opcode_Load16_Indirect},

    {"out", "reg8_reg8", 2, Opcode_Move},

//    {"ei", "", {1, INS_CONTROL | (CONTROL_HASH << INS_P3)}, {-1, 0, 0}},
//    {"di", "", {1, INS_CONTROL | (CONTROL_HASH << INS_P3)}, {-1, 0, 0}},
//    {"clc", "", {1, INS_CONTROL | (CONTROL_HASH << INS_P3)}, {-1, 0, 0}},
//    {"stc", "", {1, INS_CONTROL | (CONTROL_HASH << INS_P3)}, {-1, 0, 0}},


    {NULL, NULL, 0, 0}
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

static void copy_immediate8(unsigned char *opcode, char start, char *str, char **symbolName, char *symbolStart, char *symbolLen)
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

static void copy_immediate16(unsigned char *opcode, char start, char *str, char **symbolName, char *symbolStart, char *symbolLen)
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

static void Opcode_Nop(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = INS_CONTROL | (CONTROL_FINISH << INS_P3);
}

static void Opcode_Halt(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = INS_CONTROL | (CONTROL_HASH << INS_P3);
}

static void Opcode_Move(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = INS_TRANSFER8;
    opcode[1] = (getRegisterIndex(args->at(1)) << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1);
}

static void Opcode_Load8(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = INS_TRANSFER8;
    opcode[1] = (REG_MEMD_FROM_PC << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1);
    copy_immediate8(opcode, 2, args->at(1), symbolName, symbolStart, symbolLen);
}

static void Opcode_Load16(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = INS_TRANSFER8;
    opcode[1] = (REG_MEMD_FROM_PC << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1);
    copy_immediate16(opcode, 2, args->at(1), symbolName, symbolStart, symbolLen);
}

static void Opcode_Load8_Indirect_Reg(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = INS_TRANSFER8_IND | (REG_MEMD_FROM_MAR << INS_P3);
    opcode[1] = (getRegisterIndex(args->at(1)) << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1);
}

static void Opcode_Load8_Indirect(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen)
{
    opcode[0] = INS_TRANSFER8_IND | (REG_MEMD_FROM_MAR << INS_P3);
    opcode[1] = (getRegisterIndex(args->at(1)) << INS_P2) | (getRegisterIndex(args->at(0)) << INS_P1);
    copy_immediate16(opcode, 2, args->at(1), symbolName, symbolStart, symbolLen);
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

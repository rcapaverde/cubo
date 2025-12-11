#ifndef ISA1_H
#define ISA1_H

typedef struct
{
    const char index;
    const char *name;
    const char *type;
    char user;
} Register;

typedef void (* OpcodeBuilder)(unsigned char *opcode, std::vector<char *> *args, char **symbolName, char *symbolStart, char *symbolLen);

typedef struct
{
    const char *name;
    const char *signature;
    int size;
    const unsigned short opcode;
    OpcodeBuilder builder;
} Instruction;

const Instruction *findInstructionByName(char *instruction, char *types);
const char *normalizeRegisterName(const char *name, char user);
const char *getRegisterType(const char *name);
const char getRegisterIndex(const char *name);

#endif

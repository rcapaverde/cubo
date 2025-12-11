#ifndef PARSER_H
#define PARSER_H

#define OBJECT_NODE_OPCODE      1
#define OBJECT_NODE_SYMBOL      2
#define OBJECT_NODE_ADDRESS     3
#define OBJECT_NODE_DATA        4

typedef struct 
{
    char type;

    union
    {
        struct {
            char size;
            unsigned char *bytes;
            char *value;
            char byte;
            char len;
        } opcode;

        struct {
            char *name;
            char constant;
            char *value;
        } symbol;

        struct {
            unsigned short value;
        } addr;

        struct {
            char len;
            unsigned short count;
            char **value;
        } data;
    } d;
} ObjectNode;

typedef struct
{
    char *source_name;
    char *name;
    std::vector<ObjectNode *> nodes;
} ObjectSegment;

typedef struct
{
    char *source_name;
    ObjectSegment *active_segment;
    std::vector<ObjectSegment *> segments;
    std::vector<ObjectNode *> symbols;
} ObjectFile;

ObjectFile *parseAsmFile(char *sourceFilename, FILE *errorFile);
ObjectNode *findSymbolByName(const ObjectFile *objectFile, const char *symbol);
ObjectSegment *findSegmentByName(const ObjectFile *objectFile, const char *segment_name);
ObjectSegment *createObjectSegment(const char *name, const char *source_file);

#endif

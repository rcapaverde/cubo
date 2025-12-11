#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>

#include "util.h"
#include "parser.h"
#include "objfile.h"

static void obj_write_short(FILE *fp, unsigned short value)
{
    unsigned char short_buffer[2];
    short_buffer[0] = (value >> 8) & 0xFF;
    short_buffer[1] = value & 0xFF;
    fwrite(short_buffer, sizeof(short_buffer), 1, fp);
}

static void obj_write_string(FILE *fp, const char *value)
{
    obj_write_short(fp, value ? strlen(value) : 0);
    if (value)
        fwrite(value, 1, strlen(value), fp);
}

char saveObjFile(const ObjectFile *objectFile, const char *filename)
{
    // conta o n�mero de segmentos
    int nSegments = objectFile->segments.size();

    if (nSegments > 0xFFFF)
    {
        fprintf(stderr, "object file exceeds the allowed number of segments\n");
        return false;
    }

    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "failed opening '%s' (OS error %d)\n", filename, errno);
        return false;
    }

    // assinatura do arquivo
    fprintf(fp, OBJFILE_SIGNATURE);

    unsigned char char_buffer;

    // quantidade total de segmentos no arquivo
    obj_write_short(fp, nSegments);

    // dump de cada segmento
    for (ObjectSegment *segment : objectFile->segments)
    {
        obj_write_string(fp, segment->source_name);
        obj_write_string(fp, segment->name);

        int nNodos = segment->nodes.size();

        if (nNodos > 0xFFFF)
        {
            fprintf(stderr, "segment '%s' exceeds the allowed number of nodes\n", segment->name);
            fclose(fp);
            return false;
        }

        obj_write_short(fp, nNodos);

        for (ObjectNode *node : segment->nodes)
        {
            fwrite(&node->type, 1, 1, fp);

            switch (node->type)
            {
            case OBJECT_NODE_OPCODE:
                fwrite(&node->d.opcode.size, 1, 1, fp);
                fwrite(node->d.opcode.bytes, node->d.opcode.size, 1, fp);

                obj_write_string(fp, node->d.opcode.value);

                if (node->d.opcode.value)
                {
                    fwrite(&node->d.opcode.byte, 1, 1, fp);
                    fwrite(&node->d.opcode.len, 1, 1, fp);
                }
                break;

            case OBJECT_NODE_SYMBOL:
                obj_write_string(fp, node->d.symbol.name);
                char_buffer = node->d.symbol.constant ? 1 : 0;
                fwrite(&char_buffer, 1, 1, fp);
                obj_write_string(fp, node->d.symbol.value != NULL ? node->d.symbol.value : "");
                break;

            case OBJECT_NODE_ADDRESS:
                obj_write_short(fp, node->d.addr.value);
                break;

            case OBJECT_NODE_DATA:
                obj_write_short(fp, node->d.data.len);
                obj_write_short(fp, node->d.data.count);

                for (int iArg = 0; iArg < node->d.data.count; iArg++)
                {
                    char *data = node->d.data.value[iArg];
                    obj_write_string(fp, data);
                }
                break;
                
            default:
                fprintf(stderr, "unknown node type in segment '%s'\n", segment->name);
                fclose(fp);
                return false;
            }
        }
    }

    return true;
}

char obj_read_short(FILE *fp, int *value)
{
    unsigned char short_buffer[2];
    if (fread(short_buffer, sizeof(short_buffer), 1, fp) != sizeof(short_buffer))
        return false;
    *value = (short_buffer[1] << 8) | short_buffer[0];
    return false;
}

char *obj_read_string(FILE *fp)
{
    int str_len;
    if (!obj_read_short(fp, &str_len))
        return NULL;
    char *str = (char *)malloc(str_len + 1);
    if (!str)
        return NULL;
    fread(str, 1, str_len, fp);
    str[str_len - 1] = '\0';
    return str;
}

ObjectFile *loadObjFile(const char *filename)
{
    ObjectFile *object_file = NULL;

    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        fprintf(stderr, "failed opening '%s' (OS error %d)\n", filename, errno);
        return NULL;
    }

    char file_signature[8];

    // assinatura do arquivo
    fread(file_signature, sizeof(file_signature), 1, fp);
    if (strncmp(file_signature, OBJFILE_SIGNATURE, sizeof(file_signature)) != 0)
    {
        fclose(fp);
        fprintf(stderr, "invalid signature\n");
        return NULL;
    }

    object_file = (ObjectFile *)malloc(sizeof(ObjectFile));
    memset(object_file, 0, sizeof(ObjectFile));

    char *symbol_name;

    // quantidade total de segmentos no arquivo
    int nSegments;
    if (!obj_read_short(fp, &nSegments))
        goto handle_unexpected_end_of_file;

    for (int iSegment = 0; iSegment < nSegments; iSegment++)
    {
        char *source_file = obj_read_string(fp);
        char *name = obj_read_string(fp);

        if (source_file == NULL || name == NULL)
            goto handle_unexpected_end_of_file;

        ObjectSegment *object_segment = createObjectSegment(name, source_file);
        object_file->segments.push_back(object_segment);

        int nNodos;
        if (!obj_read_short(fp, &nNodos))
            goto handle_unexpected_end_of_file;

        for (int iNodo = 0; iNodo < nNodos; iNodo++)
        {
            ObjectNode *object_node = (ObjectNode *)malloc(sizeof(ObjectNode));
            memset(object_node, 0, sizeof(ObjectNode));

            object_segment->nodes.push_back(object_node);

            if (fread(&object_node->type, 1, 1, fp) != 1)
                goto handle_unexpected_end_of_file;

            switch (object_node->type)
            {
            case OBJECT_NODE_OPCODE:
                if (fread(&object_node->d.opcode.size, 1, 1, fp) != 1 ||
                    fread(object_node->d.opcode.bytes, object_node->d.opcode.size, 1, fp) != (unsigned int)object_node->d.opcode.size)
                {
                    goto handle_unexpected_end_of_file;
                }

                symbol_name = obj_read_string(fp);
                if (symbol_name == NULL)
                    goto handle_unexpected_end_of_file;

                if (strlen(symbol_name) == 0)
                {
                    free(symbol_name);
                }
                else
                {
                    object_node->d.opcode.value = symbol_name;
                    if (fread(&object_node->d.opcode.byte, 1, 1, fp) != 1 ||
                        fread(&object_node->d.opcode.len, 1, 1, fp) != 1)
                    {
                        goto handle_unexpected_end_of_file;
                    }
                }
                break;

            case OBJECT_NODE_SYMBOL:
                object_node->d.symbol.name = obj_read_string(fp);
                if (object_node->d.symbol.name == NULL)
                    goto handle_unexpected_end_of_file;

                if (fread(&object_node->d.symbol.constant, 1, 1, fp) != 1)
                    goto handle_unexpected_end_of_file;

                symbol_name = obj_read_string(fp);
                if (symbol_name == NULL)
                    goto handle_unexpected_end_of_file;

                if (strlen(symbol_name) == 0)
                    free(symbol_name);
                else
                    object_node->d.symbol.value = symbol_name;
                break;

            case OBJECT_NODE_ADDRESS:
                int addr;
                if (!obj_read_short(fp, &addr))
                    goto handle_unexpected_end_of_file;
                object_node->d.addr.value = addr;
                break;

            case OBJECT_NODE_DATA:
                int datalen;
                int datacount;
                if (!obj_read_short(fp, &datalen) ||
                    !obj_read_short(fp, &datacount));
                {
                    goto handle_unexpected_end_of_file;
                }

                object_node->d.data.len = datalen;
                object_node->d.data.count = datacount;
                object_node->d.data.value = (char **)malloc(sizeof(char *) * datacount);

                for (int iArg = 0; iArg < object_node->d.data.count; iArg++)
                {
                    char *value = obj_read_string(fp);
                    if (value == NULL)
                        goto handle_unexpected_end_of_file;
                    object_node->d.data.value[iArg] = value;
                }
                break;
                
            default:
                fprintf(stderr, "unknown node type in segment '%s'\n", object_segment->name);
                goto handle_error;
            }
        }
    }

    return object_file;

handle_unexpected_end_of_file:
    fprintf(stderr, "unexpected end of file\n");

handle_error:
    fclose(fp);
    if (object_file)
    {
        // TODO
        // liberar a mem�ria alocada
    }
    
    return NULL;
}

char mergeObjFile(ObjectFile *object_file, const ObjectFile *merge_object_file)
{
    char result = true;

    // lista de s�mbolos que foram renomeados
    std::map<std::string, char *> renamed_symbols;

    // testa a exist�ncia de s�mbolos em duplicidade entre os dois arquivos objeto
    // s�mbolos locais duplicados ser�o renomeados
    // duplicidade encontrada em s�mbolos globais provocar� um erro
    for (ObjectNode *symbol : merge_object_file->symbols)
    {
        if (findSymbolByName(object_file, symbol->d.symbol.name) == NULL)
        {
            object_file->symbols.push_back(symbol);
        }
        else
        {
            if (symbol->d.symbol.name[0] == '_')
            {
                // cria um nome �nico
                unsigned int unique = 2;
                char new_name[32];
                while (true)
                {
                    strncpy(new_name, symbol->d.symbol.name, sizeof(new_name) - 10);
                    new_name[sizeof(new_name) - 10] = '\0';
                    sprintf(&new_name[strlen(new_name)], "_%x", unique++);
                    if (findSymbolByName(object_file, new_name) == NULL)
                        break;
                }
                
                // adicionar na lista de nomes antigos e novos
                char *old_name = symbol->d.symbol.name;
                symbol->d.symbol.name = strdup(new_name);
                renamed_symbols[old_name] = symbol->d.symbol.name;

                // adiciona o s�mbolo � lista do objeto resultante
                object_file->symbols.push_back(symbol);
            }
            else
            {
                fprintf(stderr, "duplicate symbol '%s'\n", symbol->d.symbol.name);
                result = false;
            }
        }
    }

    if (!result)
        return result;

    // junta os segmentos ao objeto mergeado
    for (ObjectSegment *object_segment : merge_object_file->segments)
    {
        // renomeia o s�mbolo local duplicado
        for (ObjectNode *node : object_segment->nodes)
        {
            switch (node->type)
            {
            case OBJECT_NODE_OPCODE:
                if (node->d.opcode.value)
                {
                    char *new_name = renamed_symbols[node->d.opcode.value];
                    if (new_name != NULL)
                    {
                        free(node->d.opcode.value);
                        node->d.opcode.value = strdup(new_name);
                    }
                } 
                break;

            case OBJECT_NODE_DATA:
                for (int iArg = 0; iArg < node->d.data.count; iArg++)
                {
                    char *data = node->d.data.value[iArg];
                    if (isValidLabelName(data))
                    {
                        char *new_name = renamed_symbols[data];
                        if (new_name != NULL)
                        {
                            free(data);
                            node->d.data.value[iArg] = strdup(new_name);
                        }
                    }
                }
                break;
            }
        }

        object_file->segments.push_back(object_segment);
    }

    return result;
}

void dumpObjFile(FILE *output, ObjectFile *object_file)
{
    int iSegment = 0;
    for (ObjectSegment *object_segment : object_file->segments)
    {
        fprintf(output, "segment[%d] %s (from: %s)\n", iSegment++, object_segment->name, object_segment->source_name);

        int iNode = 0;
        for (ObjectNode *node : object_segment->nodes)
        {
            switch (node->type)
            {
            case OBJECT_NODE_OPCODE:
                fprintf(output, "opcode[%d] ", iNode++);
                for (int iOpcode = 0; iOpcode < node->d.opcode.size; iOpcode++)
                    fprintf(output, "%2.2X", node->d.opcode.bytes[iOpcode]);

                if (node->d.opcode.value)
                    fprintf(output, " %s (%d,%d)", node->d.opcode.value, node->d.opcode.byte, node->d.opcode.len);

                fprintf(output, "\n");

                break;

            case OBJECT_NODE_SYMBOL:
                fprintf(output, "symbol[%d] %s", iNode++, node->d.symbol.name);

                if (node->d.symbol.value)
                    fprintf(output, " = '%s'", node->d.symbol.value);

                fprintf(output, "\n");

                break;

            case OBJECT_NODE_ADDRESS:
                fprintf(output, "address[%d] %d\n", iNode++, node->d.addr.value);
                break;

            case OBJECT_NODE_DATA:
                fprintf(output, "data[%d] (%d) [", iNode++, node->d.data.len);

                for (int iArg = 0; iArg < node->d.data.count; iArg++)
                {
                    char *data = node->d.data.value[iArg];

                    if (isNumber(data))
                    {
                        fprintf(output, "%d ", (int)strtol(data, NULL, 0));
                    }

                    else if (data[0] == '"')
                    {
                        fprintf(output, "\"");
                        int iStr = 1;
                        while (data[iStr])
                        {
                            if (data[iStr] >= 32 && data[iStr] <= 127)
                                fprintf(output, "%c", data[iStr]);
                            else
                                fprintf(output, "\\x%2.2x", data[iStr]);
                        }
                        fprintf(output, "\" ");
                    }

                    else if (isValidLabelName(data))
                    {
                        fprintf(output, "%s ", data);
                    }

                    else
                    {
                        fprintf(output, "? ");
                    }
                }
                fprintf(output, "]\n");

                break;
            }
        }
    }    
}

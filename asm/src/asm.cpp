#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <strings.h>

#include "parser.h"
#include "objfile.h"
#include "exefile.h"


#define ASM_VERSION "5.00.00"

#define MAX_PATH    260

char *change_file_extension(char *filename, const char *new_extension, const int max_len)
{
    for (int iname = strlen(filename) - 1; iname >= 0; iname--)
    {
        if (filename[iname] == '.')
        {
            strncpy(&filename[iname], new_extension, max_len - iname);
            return filename;
        }
    }

    strncpy(&filename[strlen(filename)], new_extension, max_len - strlen(filename) - 1);
    return filename;
}

char *extract_file_extension(char *filename)
{
    for (int iname = strlen(filename) - 1; iname >= 0; iname--)
        if (filename[iname] == '.')
            return &filename[iname];
    return NULL;
}

int main(int argc, char *argv[])
{
    printf("cubo assembler %s\n", ASM_VERSION);

    char asm_only = false;
    char *output_filename = NULL;

    std::vector<char *> input_filenames;

    for (int iArg = 1; iArg < argc; iArg++)
    {
        if (strcmp(argv[iArg], "-c") == 0)
            asm_only = true;

        else if (strcmp(argv[iArg], "-o") == 0)
        {
            if (iArg + 1 >= argc)
            {
                fprintf(stderr, "invalid argument usage\n");
                return 1;
            }

            if (output_filename)
            {
                fprintf(stderr, "output file name redefined\n");
                return 1;
            }

            output_filename = argv[++iArg];
        }

        else if (argv[iArg][0] == '-')
        {
            fprintf(stderr, "unknown argument: %s\n", argv[iArg]);
            return 1;
        }

        else
        {
            input_filenames.push_back(argv[iArg]);
        }
    }

    std::vector<ObjectFile *> objects;
    char error_found = false;

    if (input_filenames.empty())
    {
        fprintf(stderr, "specify which file names to process\n");
        goto handle_error;
    }

    for (char *filename : input_filenames)
    {
        char *extension = extract_file_extension(filename);

        if (extension != NULL)
        {
            if (strcasecmp(extension, ".s") == 0)
            {
                ObjectFile *object = parseAsmFile(filename, stderr);
                if (object == NULL)
                    error_found = true;
                else
                    objects.push_back(object);
            }

            else if (strcasecmp(extension, ".o") == 0)
            {
                ObjectFile *object = loadObjFile(filename);
                if (object == NULL)
                    error_found = true;
                else
                    objects.push_back(object);
            }
        }
    }

    if (error_found)
        goto handle_error;

    if (output_filename || !asm_only)
    {
        ObjectFile *merged_object_file = (ObjectFile *)malloc(sizeof(ObjectFile));
        memset(merged_object_file, 0, sizeof(ObjectFile));
        merged_object_file->source_name = objects.at(0)->source_name;

        // junta todos os objetos para salvar em �nico arquivo
        for (ObjectFile *object_file : objects)
            if (!mergeObjFile(merged_object_file, object_file))
                goto handle_error;

dumpObjFile(stdout, merged_object_file);

        objects.clear();
        objects.push_back(merged_object_file);
    }



    if (error_found)
        goto handle_error;

    if (asm_only)
    {
        for (ObjectFile *object_file : objects)
        {
            char obj_filename[MAX_PATH];

            if (output_filename)
            {
                strncpy(obj_filename, output_filename, MAX_PATH);
                obj_filename[MAX_PATH - 1] = '\0';
            }
            else
            {
                strncpy(obj_filename, object_file->source_name, MAX_PATH);
                obj_filename[MAX_PATH - 1] = '\0';
                change_file_extension(obj_filename, ".o", MAX_PATH);
            }

            if (!saveObjFile(object_file, obj_filename))
                goto handle_error;
        }
    }

    else
    {
        char exe_filename[MAX_PATH];

        if (output_filename)
        {
            strncpy(exe_filename, output_filename, MAX_PATH);
            exe_filename[MAX_PATH - 1] = '\0';
        }
        else
        {
            strncpy(exe_filename, objects.at(0)->source_name, MAX_PATH);
            exe_filename[MAX_PATH - 1] = '\0';
            change_file_extension(exe_filename, ".out", MAX_PATH);
        }

        saveExeFileHex(objects.at(0), exe_filename);
    }

    return 0;

handle_error:
    return 1;    
}



#ifndef OBJFILE_H
#define OBJFILE_H

#define OBJFILE_SIGNATURE "OBJFILE1"

char saveObjFile(const ObjectFile *object_file, const char *filename);
char mergeObjFile(ObjectFile *object_file, const ObjectFile *merge_object_file);
ObjectFile *loadObjFile(const char *filename);
void dumpObjFile(FILE *output, ObjectFile *object_file);

#endif

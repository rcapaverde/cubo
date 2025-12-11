#include <stdlib.h>
#include <string.h>

char *strdup(const char *str)
{
    char *dup = (char *)malloc(strlen(str) + 1);
    return strcpy(dup, str);
}

char isWhitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

char isNumber(const char *number)
{
    if (number[0] == '\0' || !(number[0] >= '0' && number[0] <= '9'))
        return false;

    char hex = strncmp(number, "0x", 2) == 0 || strncmp(number, "0X", 2) == 0;
    char bin = strncmp(number, "0b", 2) == 0 || strncmp(number, "0B", 2) == 0;
    char octal = false;
    if (!hex && !bin)
        octal = number[0] == '0';

    for (int i = ((hex || bin) ? 2 : 1); number[i]; i++)
    {
        if (hex)
        {        
            if (!((number[i] >= '0' && number[i] <= '9') || 
                    (number[i] >= 'a' && number[i] <= 'f') || (number[i] >= 'A' && number[i] <= 'F')))
                return false;
        }

        else if (bin)
        {        
            if (!(number[i] >= '0' && number[i] <= '1'))
                return false;
        }

        else if (octal)
        {        
            if (!(number[i] >= '0' && number[i] <= '7'))
                return false;
        }

        else
        {        
            if (!(number[i] >= '0' && number[i] <= '9'))
                return false;
        }
    }

    return true;
}

char isValidLabelName(const char *name)
{
    if (name[0] == '\0' || !((name[0] >= 'A' && name[0] <= 'Z') || 
            (name[0] >= 'a' && name[0] <= 'z') || name[0] == '_'))
        return false;

    for (int i = 0; name[i]; i++)
        if (!((name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= 'a' && name[i] <= 'z') || 
                (name[i] >= '0' && name[i] <= '9') || name[i] == '_' || name[i] == '-' || 
                name[i] == '.'))
            return false;

    return true;
}

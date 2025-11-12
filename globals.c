
#include "globals.h"
#include <string.h>

char glob_user[300] = {0};
int glob_userId = 0;
void trim_newline(char *str)
{
    if (!str) return;
    size_t len = strlen(str);
    if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r'))
        str[len - 1] = '\0';
}
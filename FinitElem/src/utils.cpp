// utils.cpp

#include <stdio.h>
#include "utils.h"

void read_string(FILE *f, char *string) {
    do {
        fgets(string, MAXSTRLEN, f);
    } while ((string[0]=='/') || string[0]=='\n');
}

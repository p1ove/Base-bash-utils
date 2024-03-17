#ifndef CAT_H
#define CAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int openfile(char *argv, char *flags);
void putc_symbol(int c, char *flags, char *prev_c, int *number,
                 int *eline_printed);
int read_flags(int argc, char **argv, char *flags);
int combin_flags(char **argv, int argc);
void read_gnu_flags(int argc, char **argv);

#endif
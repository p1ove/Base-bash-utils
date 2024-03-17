#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUFFER 7000

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  char patterns[BUFFER];
  char file[BUFFER];
} GrepOptions;

int openfile(int argc, char *argv, GrepOptions *options, int name);
int parse_flags(int argc, char **argv, GrepOptions *options);
void check_flag_i(int *redcomp_flags, GrepOptions *options);
int check_count_files(int argc, GrepOptions *options);
void option_f(GrepOptions *options);
void print_matched_parts(const char *line, regmatch_t match);
void print_opt_o(char *string_in_file, GrepOptions *options, int regcomp_flags,
                 int name, char *argv, int option_n_count);

#endif
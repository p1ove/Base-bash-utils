#include "s21_grep.h"

int main(int argc, char **argv) {
  GrepOptions options = {0};
  if (argc > 1) {
    if (!parse_flags(argc, argv, &options)) {
      int name = check_count_files(argc, &options);
      while (optind < argc) {
        openfile(argc, argv[optind], &options, name);
        optind++;
      }
    }
  } else {
    printf("usage: [-e:ivclnhsf:o] [pattern] [file]");
  }
  return 0;
}

void check_flag_i(int *regcomp_flags, GrepOptions *options) {
  if (options->i) {
    *regcomp_flags |= REG_ICASE;
  }
}

int check_count_files(int argc, GrepOptions *options) {
  int print_name_file = 1;
  if (optind + 1 == argc || options->h) print_name_file = 0;
  return print_name_file;
}

int openfile(int argc, char *argv, GrepOptions *options, int name) {
  FILE *f;
  f = fopen(argv, "rb");
  argc = argc;
  if (f != NULL) {
    char string_in_file[BUFFER];
    regex_t regex;
    regmatch_t start;
    int reg_result;
    int regcomp_flags = REG_EXTENDED;
    int option_c_count = 0;
    int option_n_count = 1;
    int option_l_flag = 0;
    check_flag_i(&regcomp_flags, options);
    regcomp(&regex, options->patterns, regcomp_flags);
    while (fgets(string_in_file, BUFFER, f) != NULL) {
      reg_result = regexec(&regex, string_in_file, 1, &start, 0);

      if (options->v && reg_result && !options->c && !options->l) {
        if (name) {
          printf("%s:", argv);
        }
        if (options->n) {
          printf("%d:", option_n_count);
        }
        printf("%s", string_in_file);
        if (strstr(string_in_file, "\n") == NULL) printf("\n");
      }
      if (options->c && !options->v && !reg_result)
        option_c_count++;
      else if (options->c && options->v && reg_result)
        option_c_count++;

      if (options->l && !reg_result) option_l_flag = 1;

      if (!reg_result && !options->v && !options->c && !options->l &&
          options->o) {
        print_opt_o(string_in_file, options, regcomp_flags, name, argv,
                    option_n_count);
      }

      if (!reg_result && !options->v && !options->c && !options->l &&
          !options->o) {
        if (name) {
          printf("%s:", argv);
        }
        if (options->n) {
          printf("%d:", option_n_count);
        }
        printf("%s", string_in_file);
        if (strstr(string_in_file, "\n") == NULL) printf("\n");
      }
      option_n_count++;
    }
    if (options->c && !options->l) {
      if (name) {
        printf("%s:", argv);
      }
      printf("%d", option_c_count);
      printf("\n");
    }
    if (options->l && option_l_flag && !options->c && !options->v) {
      printf("%s", argv);
      printf("\n");
    }
    if (options->l && option_l_flag && options->c && !name && !options->v) {
      printf("1\n%s", argv);
      printf("\n");
    }
    if (options->l && options->v && options->c) {
      if (name) {
        printf("%s:", argv);
      }
      printf("1\n%s", argv);
      printf("\n");
    }
    if (options->l && options->c && name && !options->v) {
      if (option_l_flag) {
        printf("%s:%d\n%s", argv, option_l_flag, argv);
        printf("\n");
      }
      if (!option_l_flag) {
        printf("%s:%d", argv, option_l_flag);
        printf("\n");
      }
    }
    if (options->l && options->v && !options->c) {
      printf("%s\n", argv);
    }
    regfree(&regex);
    fclose(f);
  } else {
    if (!options->s) printf("%s: No such file or directory\n", argv);
  }
  return 0;
}

void print_opt_o(char *string_in_file, GrepOptions *options, int regcomp_flags,
                 int name, char *argv, int option_n_count) {
  regex_t regex_flag_o;
  regcomp(&regex_flag_o, options->patterns, regcomp_flags);
  regmatch_t match;
  int offset = 0;
  int check_double = 1;
  while (regexec(&regex_flag_o, string_in_file + offset, 1, &match, 0) == 0) {
    int start = match.rm_so + offset;
    int end = match.rm_eo + offset;

    if (name && check_double) {
      printf("%s:", argv);
    }
    if (options->n && check_double) {
      printf("%d:", option_n_count);
    }
    for (int j = start; j < end; j++) {
      printf("%c", string_in_file[j]);
    }
    printf("\n");
    offset = end;
    check_double = 0;
  }
  regfree(&regex_flag_o);
}

int parse_flags(int argc, char **argv, GrepOptions *options) {
  int opt;
  int invalid_option = 0;
  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", 0, 0)) != -1) {
    switch (opt) {
      case 'o':
        options->o = 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 'h':
        options->h = 1;
        break;
      case 'n':
        options->n = 1;
        break;
      case 'l':
        options->l = 1;
        break;
      case 'c':
        options->c = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      case 'i':
        options->i = 1;
        break;
      case 'f':
        options->f = 1;
        if (optarg == NULL || optarg[0] == '\0')
          invalid_option = 1;
        else
          option_f(options);
        break;
      case '?':
        invalid_option = 1;
        break;
      case 'e':
        if (options->e) strcat(options->patterns, "|");
        options->e = 1;
        if (optarg == NULL || optarg[0] == '\0')
          invalid_option = 1;
        else
          strcat(options->patterns, optarg);
        break;
      default:
        break;
    }
  }

  if (!options->e && !options->f && !invalid_option) {
    strcat(options->patterns, argv[optind]);
    optind++;
  }
  return invalid_option;
}

void option_f(GrepOptions *options) {
  FILE *f;
  char string_in_file[BUFFER] = "";
  f = fopen(optarg, "rb");
  if (f != NULL) {
    while (fgets(string_in_file, BUFFER, f) != NULL) {
      char *newline = strchr(string_in_file, '\n');
      if (newline != NULL) {
        *newline = '\0';
      }
      if (strlen(options->patterns) > 0 && strlen(string_in_file) > 1)
        strcat(options->patterns, "|");
      strcat(options->patterns, string_in_file);
    }
    fclose(f);
  }
}
#include "cat.h"

int main(int argc, char **argv) {
  int flags_count = combin_flags(argv, argc);

  read_gnu_flags(argc, argv);

  char flags[100] = "\0";
  if (read_flags(argc, argv, flags)) {
    if (flags_count + 1 == argc) openfile("-", flags);
    for (int i = flags_count + 1; i < argc; i++) openfile(argv[i], flags);
  } else
    printf("cat: illegal option\nusage: cat [-benstv] [file ...]");
  return 0;
}

int combin_flags(char **argv, int argc) {
  int flags;
  int count;
  for (count = 1, flags = 0; count < argc; count++) {
    if (strchr(argv[count], '-') != NULL && strlen(argv[count]) > 1) flags++;
  }
  // printf("%d\n",flags);
  return flags;
}

void read_gnu_flags(int argc, char **argv) {
  char *flag;
  for (int i = 0; i < argc; i++) {
    if ((flag = strstr(argv[i], "--number-nonblank")) != NULL) argv[i] = "-b";
    if ((flag = strstr(argv[i], "--number")) != NULL) argv[i] = "-n";
    if ((flag = strstr(argv[i], "--squeeze-blank")) != NULL) argv[i] = "-s";
  }
}

int read_flags(int argc, char **argv, char *flags) {
  int count, count_f = 0;
  int legal_option = 1;
  for (count = 1; count < argc; count++) {
    if (argv[count][0] != '-' || !strcmp(argv[count], "-"))
      break;
    else {
      for (size_t i = 0; i < strlen(argv[count]); i++, count_f++) {
        if ((argv[count][i] != ' ') && (argv[count][i] != '-') &&
            (argv[count][i] != 's') && (argv[count][i] != 'n') &&
            (argv[count][i] != 'b') && (argv[count][i] != 'e') &&
            (argv[count][i] != 't') && (argv[count][i] != 'v') &&
            (argv[count][i] != 'T') && (argv[count][i] != 'E'))
          legal_option = 0;
        if ((argv[count][i] == 'e') || (argv[count][i] == 't')) {
          flags[count_f] = 'v';
          count_f++;
        }
        flags[count_f] = argv[count][i];
      }
    }
  }
  return legal_option;
}

int openfile(char *argv, char *flags) {
  // printf("%s",argv);
  FILE *f;
  if (strcmp(argv, "-") == 0)
    f = stdin;
  else
    f = fopen(argv, "rb");
  if (f != NULL) {
    char prev_c = '\n';
    int number = 0;
    int eline_printed = 0;
    char c = fgetc(f);
    while (c != EOF) {
      putc_symbol(c, flags, &prev_c, &number, &eline_printed);
      c = fgetc(f);
    }
    if (f != stdin) fclose(f);
  } else {
    printf("\n%s: No such file or directory\n", argv);
  }
  return 0;
}

void putc_symbol(int c, char *flags, char *prev_c, int *number,
                 int *eline_printed) {
  char *flag;
  if ((((flag = strchr(flags, 's')) != NULL)) &&
      (c == '\n' && *prev_c == '\n') && *eline_printed) {
  } else {
    if (*prev_c == '\n' && c == '\n')
      *eline_printed = 1;
    else
      *eline_printed = 0;

    if ((((flag = strchr(flags, 'n')) != NULL)) &&
        (((flag = strchr(flags, 'b')) == NULL)) && (*prev_c == '\n')) {
      *number += 1;
      printf("%6d\t", *number);
    }

    if ((((flag = strchr(flags, 'b')) != NULL)) &&
        ((*prev_c == '\n') || (*prev_c == -100)) && (c != '\n')) {
      *number += 1;
      printf("%6d\t", *number);
    }

    if ((strchr(flags, 'E') != NULL || strchr(flags, 'e') != NULL) && c == '\n')
      printf("$");

    if ((strchr(flags, 'T') != NULL || strchr(flags, 't') != NULL) &&
        c == '\t') {
      printf("^");
      c = '\t' + 64;
    }

    if (strchr(flags, 'v') != NULL) {
      if (c > 127 && c < 160) printf("M-^");
      if ((c < 32 && c != '\n' && c != '\t') || c == 127) printf("^");
      if ((c < 32 || (c > 126 && c < 160)) && c != '\n' && c != '\t')
        c = c > 126 ? c - 128 + 64 : c + 64;
    }
    fputc(c, stdout);
  }
  *prev_c = c;
}
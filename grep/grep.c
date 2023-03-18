#include "grep.h"

void parser(int argc, char **argv, Flags *pp) {
  int rez = 0;
  const char *short_flags = "e:ivclnhso";
  pp->pattern = my_malloc(2);
  pp->pattern[0] = '\0';
  pp->pattern[1] = '\0';
  size_t pattern_size = 0;
  opterr = 0;

  while (1) {
    rez = getopt_long(argc, argv, short_flags, 0, 0);
    if (rez == -1) break;
    pp->regex_flag |= REG_NEWLINE;
    switch (rez) {
      case 'e': {
        pp->pattern = string_append_expr(pp->pattern, &pattern_size, optarg,
                                         strlen(optarg));
        break;
      }
      case 'i':
        pp->regex_flag |= REG_ICASE;
        break;
      case 'v':
        pp->v_flag = 1;
        break;
      case 'c':
        pp->c_flag = 1;
        break;
      case 'l':
        pp->l_flag = 1;
        break;
      case 'n':
        pp->n_flag = 1;
        break;
      case 'h':
        pp->h_flag = 1;
        break;
      case 's':
        pp->s_flag = 1;
        break;
      case 'o':
        pp->o_flag = 1;
        break;
      default: {
        free(pp->pattern);
        fprintf(stderr, "Usage: ./s21_grep [OPTION]... PATTERNS [FILE]...\n");
        exit(1);
      }
    }
  }
  if (pattern_size) {
    pp->size = pattern_size;
  }
}

char *string_append_expr(char *string, size_t *size, char const *expr,
                         size_t size_expr) {
  string = my_realloc(string, *size + size_expr + 7);
  string[*size] = '\\';
  string[*size + 1] = '|';
  string[*size + 2] = '\\';
  string[*size + 3] = '(';
  memcpy(string + *size + 4, expr, size_expr);
  *size += size_expr + 4;
  string[*size] = '\\';
  string[*size + 1] = ')';
  string[*size + 2] = '\0';
  *size += 2;
  return string;
}

void *my_realloc(void *block, size_t size) {
  void *temp;
  temp = realloc(block, size);
  if (!temp) exit(errno);
  return temp;
}

void *my_malloc(size_t size) {
  void *temp;
  temp = malloc(size);
  if (!temp) exit(errno);
  return temp;
}

void grep(int argc, char **argv, Flags *pp) {
  char **end = &argv[argc];
  int count_files = 0;
  regex_t storage;
  regex_t *reg = &storage;
  if (pp->size == 0) {
    if (regcomp(reg, argv[0], pp->regex_flag)) {
      fprintf(stderr, "invalid regex pattern\n");
      exit(1);
    }
  } else {
    if (regcomp(reg, pp->pattern + 2, pp->regex_flag)) {
      fprintf(stderr, "invalid regex pattern\n");
      exit(1);
    }
  }
  free(pp->pattern);
  count_files = file_counter(argv, end, pp);
  for (char **filename = argv + (pp->size ? 0 : 1); filename != end;
       ++filename) {
    FILE *f = fopen(*filename, "rb");
    if (errno) {
      if (!pp->s_flag) {
        fprintf(stderr, "%s: ", argv[0]);
        perror(*filename);
      }
      errno = 0;
      continue;
    }
    flag_analysis(f, pp, reg, count_files, filename);
    fclose(f);
  }
  regfree(reg);
}

int file_counter(char **argv, char **end, Flags *pp) {
  int counter = 0;
  for (char **filename = argv + (pp->size ? 0 : 1); filename != end;
       ++filename) {
    counter++;
  }
  return counter;
}

void flag_analysis(FILE *f, Flags *pp, regex_t *reg, int count_files,
                   char **filename) {
  if (pp->l_flag) {
    l_option(f, pp, reg, filename);
  } else if (pp->c_flag) {
    c_option(f, pp, reg, count_files, filename);
  } else {
    search(f, pp, reg, count_files, filename);
  }
}

void search(FILE *f, Flags *pp, regex_t *reg, int count_files,
            char **filename) {
  char *line = 0;
  size_t length = 0;
  regmatch_t match;
  int count_lines = 0, i = 0, flag = 0;

  while (getline(&line, &length, f) != -1) {
    count_lines++;
    if (pp->v_flag) {
      if (regexec(reg, line, 1, &match, 0))
        if (!pp->o_flag) {
          print_filename(pp, filename, count_files);
          n_option(pp, count_lines);
          printf("%s", line);
          flag = count_lines;
        }
    } else {
      if (!regexec(reg, line, 1, &match, 0)) {
        print_filename(pp, filename, count_files);
        n_option(pp, count_lines);
        if (pp->o_flag) {
          o_option(line, match, reg, pp, count_lines, filename, count_files);
        } else {
          printf("%s", line);
          flag = count_lines;
        }
      }
    }
  }
  for (i = 0; line[i]; i++)
    ;
  if (line[i - 1] != '\n' && flag == count_lines) printf("\n");
  free(line);
}

void c_option(FILE *f, Flags *pp, regex_t *reg, int count_files,
              char **filename) {
  char *line = 0;
  size_t length = 0;
  regmatch_t match;
  int count = 0;

  while (getline(&line, &length, f) != -1) {
    if (pp->v_flag) {
      if (regexec(reg, line, 1, &match, 0)) count++;
    } else {
      if (!regexec(reg, line, 1, &match, 0)) count++;
    }
  }
  print_filename(pp, filename, count_files);
  printf("%i\n", count);
  free(line);
}

void l_option(FILE *f, Flags *pp, regex_t *reg, char **filename) {
  char *line = 0;
  size_t length = 0;
  regmatch_t match;

  while (getline(&line, &length, f) != -1) {
    if (!pp->v_flag) {
      if (!regexec(reg, line, 1, &match, 0)) {
        printf("%s\n", *filename);
        break;
      }
    } else {
      if (regexec(reg, line, 1, &match, 0)) {
        printf("%s\n", *filename);
        break;
      }
    }
  }
  free(line);
}

void o_option(char *line, regmatch_t match, regex_t *reg, Flags *pp,
              int count_lines, char **filename, int count_files) {
  printf("%.*s\n", (int)(match.rm_eo - match.rm_so), line + match.rm_so);
  char *remain = line + match.rm_eo;
  while (!regexec(reg, remain, 1, &match, 0)) {
    print_filename(pp, filename, count_files);
    n_option(pp, count_lines);
    printf("%.*s\n", (int)(match.rm_eo - match.rm_so), remain + match.rm_so);
    remain = remain + match.rm_eo;
  }
}

void n_option(Flags *pp, int count_lines) {
  if (pp->n_flag) {
    printf("%i", count_lines);
    printf(":");
  }
}

void print_filename(Flags *pp, char **filename, int count_files) {
  if (!pp->h_flag && count_files > 1) {
    printf("%s", *filename);
    printf(":");
  }
}

int main(int argc, char **argv) {
  Flags flags = {0};
  Flags *pp = &flags;
  if (argc < 3) {
    fprintf(stderr, "Usage: ./s21_grep [OPTION]... PATTERNS [FILE]...\n");
    exit(1);
  }
  parser(argc, argv, pp);
  argv += optind;
  argc -= optind;
  grep(argc, argv, pp);

  return 0;
}
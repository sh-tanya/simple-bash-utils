#ifndef GREP_H
#define GREP_H

#define _GNU_SOURCE

#include <errno.h>
#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Flags {
  char *pattern;
  size_t size;
  int e_flag;
  int regex_flag;
  int v_flag;
  int c_flag;
  int l_flag;
  int n_flag;
  int h_flag;
  int s_flag;
  int o_flag;
} Flags;

void parser(int argc, char **argv, Flags *pp);
void *my_realloc(void *block, size_t size);
void *my_malloc(size_t size);
char *string_append_expr(char *string, size_t *size, char const *expr,
                         size_t size_expr);
void search(FILE *f, Flags *pp, regex_t *reg, int count_files, char **filename);
void grep(int argc, char **argv, Flags *pp);
int file_counter(char **pattern, char **end, Flags *pp);
void flag_analysis(FILE *f, Flags *pp, regex_t *reg, int count_files,
                   char **filename);
void c_option(FILE *f, Flags *pp, regex_t *reg, int count_files,
              char **filename);
void l_option(FILE *f, Flags *pp, regex_t *reg, char **filename);
void o_option(char *line, regmatch_t match, regex_t *reg, Flags *pp,
              int count_lines, char **filename, int count_files);
void n_option(Flags *pp, int count_lines);
void print_filename(Flags *pp, char **filename, int count_files);

#endif  // GREP_H
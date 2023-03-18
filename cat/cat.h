#ifndef CAT_H
#define CAT_H

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Flags {
  int b_flag;
  int e_flag;
  int s_flag;
  int t_flag;
  int n_flag;
  int flag_T;
  int flag_E;
  int v_flag;
  int cur_file;
  char prev_ch;
} Flags;

void parser(int argc, char **argv, Flags *pp);
char print(FILE *f, Flags *pp, int *counter, int argc);
void reader(int argc, char **argv, Flags *pp);
bool eof_check(int argc, Flags *pp, int **counter);
int v_option(char c);
void t_option(char cur_char, int *check, Flags *pp, int *v_res);
void e_option(char cur_char, Flags *pp, int *v_res);

#endif  // CAT_H
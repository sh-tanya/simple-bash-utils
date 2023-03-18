#include "cat.h"

void parser(int argc, char **argv, Flags *pp) {
  int rez, option_index = 0;

  const struct option long_flags[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {NULL, 0, NULL, 0}};

  const char *short_flags = "bestnvTE";

  while (1) {
    rez = getopt_long(argc, argv, short_flags, long_flags, &option_index);
    if (rez == -1) break;
    switch (rez) {
      case 'b':
        pp->b_flag = 1;
        break;
      case 's':
        pp->s_flag = 1;
        break;
      case 'n':
        pp->n_flag = 1;
        break;
      case 'v':
        pp->v_flag = 1;
        break;
      case 'T':
        pp->flag_T = 1;
        break;
      case 'E':
        pp->flag_E = 1;
        break;
      case 't': {
        pp->t_flag = 1;
        pp->v_flag = 1;
        break;
      }
      case 'e': {
        pp->e_flag = 1;
        pp->v_flag = 1;
        break;
      }
      default:
        fprintf(stderr, "invalid option\n");
        exit(1);
    }
  }
}

void reader(int argc, char **argv, Flags *pp) {
  FILE *f = NULL;
  int counter = 1;
  pp->cur_file = optind;

  while (pp->cur_file <= argc) {
    if (pp->cur_file != argc) {
      f = fopen(argv[pp->cur_file], "r");
      if (!f) {
        fprintf(stderr, "%s: %s: no such file or directory\n", argv[0],
                argv[pp->cur_file]);
        exit(1);
      }
      pp->prev_ch = print(f, pp, &counter, argc);
      fclose(f);
    }
    pp->cur_file += 1;
  }
}

bool eof_check(int argc, Flags *pp, int **counter) {
  int flag = optind, file_count = 0;
  bool new_line = true;

  for (flag = optind; flag <= argc; flag++) file_count++;

  if (file_count > 2 && pp->prev_ch != '\n' && **counter != 1) {
    new_line = false;
  }
  return new_line;
}

char print(FILE *f, Flags *pp, int *counter, int argc) {
  char cur_ch = 0, prev2 = 0;
  int v_res = 0, check = 0;
  bool new_line = eof_check(argc, pp, &counter);
  if (*counter == 1) pp->prev_ch = '\n';

  while ((cur_ch = fgetc(f == NULL ? stdin : f)) != EOF) {
    if (pp->s_flag == 1)
      if (pp->prev_ch == '\n' && cur_ch == '\n' && prev2 == '\n') continue;

    if (pp->b_flag == 1) {
      if (pp->prev_ch == '\n' && cur_ch != '\n') {
        printf("%6d\t", *counter);
        *counter += 1;
      }
    } else if (pp->n_flag == 1) {
      if (new_line == true) {
        printf("%6d\t", *counter);
        *counter += 1;
      }
      new_line = (cur_ch == '\n');
    } else {
      *counter = 0;
    }

    if (pp->e_flag == 1 || pp->flag_E == 1) e_option(cur_ch, pp, &v_res);

    if (pp->t_flag == 1 || pp->flag_T == 1)
      t_option(cur_ch, &check, pp, &v_res);

    if (pp->v_flag == 1 && pp->e_flag == 0 && pp->t_flag == 0)
      v_res = v_option(cur_ch);

    if (v_res == 0 && check == 0) putc(cur_ch, stdout);

    prev2 = pp->prev_ch;
    pp->prev_ch = cur_ch;
    check = 0;
  }
  return pp->prev_ch;
}

int v_option(char c) {
  int result = 1;

  if (c < -96) {
    fprintf(stdout, "M-^%c", c + 192);
  } else if (c < 0) {
    fprintf(stdout, "M-%c", c + 128);
  } else if (c == 9 || c == 10) {
    result = 0;
  } else if (c < 32) {
    fprintf(stdout, "^%c", c + 64);
  } else if (c < 127) {
    result = 0;
  } else {
    fprintf(stdout, "^?");
  }
  return result;
}

void t_option(char cur_char, int *check, Flags *pp, int *v_res) {
  if (pp->t_flag == 1 && pp->e_flag == 0) *v_res = v_option(cur_char);
  if (cur_char == 9) {
    fputs("^I", stdout);
    *check = 1;
  }
}

void e_option(char cur_char, Flags *pp, int *v_res) {
  if (pp->e_flag == 1) *v_res = v_option(cur_char);
  if (cur_char == '\n') putc('$', stdout);
}

int main(int argc, char **argv) {
  Flags flags = {0};
  Flags *pp = &flags;
  parser(argc, argv, pp);
  reader(argc, argv, pp);
  return 0;
}
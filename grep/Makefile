NAME = s21_grep

CFLAGS = -Wall -Wextra -Werror -std=c11

HEADER = grep.h

SOURCE = grep.c

REMOVE = rm -f

NAME_GREP:
			gcc ${CFLAGS} ${SOURCE} -Include${HEADER} -o ${NAME}

s21_grep:   NAME_GREP

clean:      
			${REMOVE} ${NAME}

rebuild:    clean s21_grep
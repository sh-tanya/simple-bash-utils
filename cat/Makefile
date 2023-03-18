NAME = s21_cat

CFLAGS = -Wall -Wextra -Werror -std=c11

HEADER = cat.h

SOURCE = cat.c

REMOVE = rm -f

NAME_CAT:
			gcc ${CFLAGS} ${SOURCE} -Include${HEADER} -o ${NAME}

s21_cat:	NAME_CAT

clean:		
			${REMOVE} ${NAME}

rebuild:	clean s21_cat	
TARGET = 3.1
SRCS = cp_r.c

CC=gcc
RM=rm
CFLAGS= -g -Wall -Werror -Wextra
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET} 

${TARGET}: cp_r.h ${SRCS}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS} ${LIBS} -o ${TARGET}

clean:
	${RM} -f *.o ${TARGET}

CC=/opt/homebrew/bin/gcc-12
CFLAGS = -Wall -g
PROG= main
OBJS= bosh_utilities.o bosh.o bosh_parser.o bosh_errhand.o
LFLAGS= -lreadline
.o: .c
	${CC} ${CFLAGS} -c $^
all: ${PROG}
pipe: pipetut.o
	${CC} ${CFLAGS} -o pipe pipetut.o
${PROG}: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} ${LFLAGS} -o main
clean:
	rm -f *.o main pipe
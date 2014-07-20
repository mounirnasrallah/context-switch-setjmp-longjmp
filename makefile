CC=gcc
CFLAGS=-Wall 
BIN=bin
LIB=lib
OBJ=obj
SRC=src
INC=include



all: directories main

directories: ${OBJ} ${BIN} ${LIB}

${OBJ}:
	mkdir ${OBJ}
${BIN}:
	mkdir ${BIN}
${LIB}:
	mkdir ${LIB}


${OBJ}/main.o: ${SRC}/main.c ${INC}/header.h
	${CC} -o $@ -c $< ${CFLAGS} -I${INC}

${BIN}/main: ${OBJ}/main.o
	${CC} -o $@ $< ${CFLAGS}

main: ${BIN}/main

clean:
	rm -f ${OBJ}/* ${BIN}/* ${LIB}/*

cleanall:
	rm -rf ${OBJ} ${BIN} ${LIB}
	rm -f ${INC}/*~ ${SRC}/*~ *~
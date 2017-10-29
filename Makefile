PLATFORM=$(shell util/platform.sh)

SRC=src
PSRC=posixino
OBJ=/tmp/tabletenniscounter/${PLATFORM}
BIN=bin/${PLATFORM}

CCC=g++ -std=c++11 -I${PSRC} -O3
CC=${CCC} -c
CL=${CCC} -pthread

OBJLIST=\
	${OBJ}/posixino.o  \
	${OBJ}/ttc.o


${BIN}/ttc: \
		Makefile \
		${BIN} ${OBJ} ${OBJLIST} \
		${SRC}/ttc.hpp ${SRC}/ttc.cpp
	${CL} \
		${OBJLIST} \
		-o ${BIN}/ttc

${OBJ}/posixino.o: \
		Makefile \
		${PSRC}/posixino.hpp \
		${PSRC}/posixino.cpp
	${CC} \
		-D TIMER1="(100 * 1000)" \
		-o ${OBJ}/posixino.o \
		${PSRC}/posixino.cpp

${OBJ}/ttc.o: \
		Makefile \
 		${SRC}/ttc.hpp \
 		${SRC}/ttc.cpp
	${CC} \
		-o ${OBJ}/ttc.o \
		${SRC}/ttc.cpp

${BIN}:
	mkdir -p ${BIN}

${OBJ}:
	mkdir -p ${OBJ}

clean: ${OBJ}
	rm -rf ${OBJ}/*.o

all: clean ${BIN}/ttc

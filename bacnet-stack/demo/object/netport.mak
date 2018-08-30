#Makefile to build test case
CC      = gcc
SRC_DIR = ../../src
TEST_DIR = ../../test
INCLUDES = -I../../include -I$(TEST_DIR) -I.
DEFINES = -DBIG_ENDIAN=0 -DBACNET_UNIT_TEST -DBACAPP_ALL -DTEST_NETWORK_PORT

CFLAGS  = -Wall $(INCLUDES) $(DEFINES) -g

SRCS = netport.c \
	$(SRC_DIR)/bacdcode.c \
	$(SRC_DIR)/bacint.c \
	$(SRC_DIR)/bacstr.c \
	$(SRC_DIR)/bacreal.c \
	$(SRC_DIR)/datetime.c \
	$(SRC_DIR)/bacapp.c \
	$(SRC_DIR)/bacdevobjpropref.c \
	$(SRC_DIR)/bactext.c \
	$(SRC_DIR)/lighting.c \
	$(SRC_DIR)/indtext.c \
	$(TEST_DIR)/ctest.c

TARGET = network_port

all: ${TARGET}

OBJS = ${SRCS:.c=.o}

${TARGET}: ${OBJS}
	${CC} -o $@ ${OBJS}

.c.o:
	${CC} -c ${CFLAGS} $*.c -o $@

depend:
	rm -f .depend
	${CC} -MM ${CFLAGS} *.c >> .depend

clean:
	rm -rf core ${TARGET} $(OBJS)

include: .depend

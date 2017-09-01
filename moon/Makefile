TARGET  := moon
WARN    := -Wall 
CFLAGS  := -O3 ${WARN}
LDFLAGS := -lm
CC      := gcc


all: ${TARGET}

${TARGET}.o: ${TARGET}.c
${TARGET}: ${TARGET}.o

clean:
	rm -rf ${TARGET}.o ${TARGET}

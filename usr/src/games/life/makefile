# @(#)makefile	1.3	1/31/85
# @(#)

CFLAGS = -O
CC = cc

CFILES = alloc.c cell.c cmd.c cmdl.c debug.c gen.c io.c main.c mark.c\
	object.c scan.c vars.c view.c

OFILES = alloc.o cell.o cmd.o cmdl.o debug.o gen.o io.o main.o mark.o\
	object.o scan.o vars.o view.o

life:	${OFILES} makefile
	${CC} ${CFLAGS} -o life ${OFILES} -ldpy -ltermlib

${OFILES}: life.h

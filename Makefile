# Variables
CC=gcc
CLINK=$(CC)
CFLAGS=-g -Wall -std=c99 -c -fpic
OBJS=grades.o
RM=rm -rf *.o

libgrades.so: grades.o
	$(CLINK) -shared $(OBJS) -o libgrades.so -llinked-list -L.

grades.o: grades.c grades.h
	$(CC) $(CFLAGS) grades.c

clean:
	$(RM) libgrades.so
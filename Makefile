CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -g
LDFLAGS=-g
TARGETS=fish cmdline_test

all: $(TARGETS)

#executable Fish
fish : fish.o cmdline.o streams.o
	$(CC) $(LDFLAGS) $^ -o $@
fish.o : fish.c cmdline.h streams.h
	$(CC) $(CFLAGS) -c -o $@ $<

#executable cmdline_test
cmdline_test : cmdline_test.o cmdline.o
	$(CC) $(LDFLAGS) $^ -o $@
cmdline_test.o : cmdline_test.c cmdline.h 
	$(CC) $(CFLAGS) -c -o $@ $<

#Funtions used for executables
cmdline.o : cmdline.c cmdline.h
	$(CC) $(CFLAGS) -c -o $@ $<
streams.o : streams.c streams.h
	$(CC) $(CFLAGS) -c -o $@ $<



clean : 
	rm -f *.o
mrproper : clean
	rm -f fish cmdline_test
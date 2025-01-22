# file name
BINARY := njtraffic

# directories
BINDIR := bin
LIBDIR := lib
SRCDIR := src
TESTDIR := test

# compiler
CC := gcc

# GCC standard
STD := -std=c18

# warnings
WARNS := -Wall -Wextra -pedantic

#includes
INCS := -I$(SRCDIR) -I$(LIBDIR)

# flags
CFLAGS := -O3 $(STD) $(WARNS) $(INCS)

# cmocka
CMOCKA := -l cmocka -L /usr/lib

# tests binary file
TEST_BINARY := $(BINARY)_test_runner

# c files
FILES := $(wildcard $(SRCDIR)/*.c) $(wildcard $(LIBDIR)/*/*.c)

### Make Options ###

default: all

help:
	@echo "Traffic Light State Machine"
	@echo
	@echo "Target rules:"
	@echo "    all      - Compiles and builds binary for normal operation"
	@echo "    tests    - Compiles with cmocka, builds and executes tests binary"
	@echo "    clean    - Clean the project"
	@echo "    help     - Prints this message"

# Build normal binary
all:
	$(CC) -o $(BINDIR)/$(BINARY) $(FILES) $(CFLAGS)
	@echo "Binary file : $(BINDIR)/$(BINARY)";

# Build and run test binary
tests:
	$(CC) $(TESTDIR)/main.c -o $(BINDIR)/$(TEST_BINARY) $(CFLAGS) $(CMOCKA)
	@echo "Running tests: ";
	./$(BINDIR)/$(TEST_BINARY)

clean:
	@rm -rvf $(BINDIR)/*

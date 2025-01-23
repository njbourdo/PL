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
MAIN_FILES := $(SRCDIR)/main.*
APP_FILES := $(filter-out $(SRCDIR)/main.c, $(wildcard $(SRCDIR)/*.c)) $(wildcard $(LIBDIR)/*/*.c)
TEST_FILES := $(wildcard $(TESTDIR)/*.c)

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
	$(CC) -o $(BINDIR)/$(BINARY) $(MAIN_FILES) $(APP_FILES) $(CFLAGS)
	@echo "Binary file : $(BINDIR)/$(BINARY)";

# Build and run test binary
tests:
	$(CC) -o $(BINDIR)/$(TEST_BINARY) $(TEST_FILES) $(APP_FILES) $(CFLAGS) $(CMOCKA) -DTESTING
	@echo "Running tests: ";
	./$(BINDIR)/$(TEST_BINARY)

clean:
	@rm -rvf $(BINDIR)/*

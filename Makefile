# Project name
NAME = compiler

# Folders
SRCFOLDER := src
OBJFOLDER := obj
TESTFOLDER := tests

# Files
SRCFILES := $(wildcard $(SRCFOLDER)/*.c)
OBJFILES := $(patsubst $(SRCFOLDER)/%.c, $(OBJFOLDER)/%.o, $(SRCFILES))

# Get all test files in nested directories
#TESTFILES := $(wildcard $(TESTFOLDER)/*/*.c)
#TESTBINS := $(TESTFILES:$(TESTFOLDER)/%.c=$(TESTFOLDER)/%)

INTERPRETER := ic24int
IFJCODE := out.ifjcode
INZIG := in.ifj

TESTFOLDER = ./tests
TESTSCRIPT = ./tests/test.sh

# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g

# Default target
all: $(NAME)

# Rule for building in submitted directory
$(NAME): $(wildcard *.c) $(wildcard *.h)
	$(CC) $(CFLAGS) $(wildcard *.c) -o $(NAME)

# Rule to build the executable in developer space
dev: $(OBJFILES)
	mkdir -p $(OBJFOLDER)
	$(CC) $(CFLAGS) $(OBJFILES) -o $(NAME)

# Rule to compile .c files into .o files
$(OBJFOLDER)/%.o: $(SRCFOLDER)/%.c
	mkdir -p $(OBJFOLDER)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the compiled files and the executable
clean:
	rm -rf $(OBJFOLDER)/
	rm -f $(NAME)
	rm -f $(TESTFOLDER)/out/* -R

# Run tests
test: $(NAME) $(INTERPRETER)
	$(TESTSCRIPT) $(TESTFOLDER) $(NAME) $(INTERPRETER)

# Run the main executable

pack:
	zip -j xnovakf00.zip ./src/*.c ./src/*.h ./rozdeleni ./rozsireni ./dokumentace.pdf ./Makefile

$(IFJCODE):
	touch $(IFJCODE)

$(INZIG):
	touch $(INZIG)

run: $(NAME)
	./$(NAME) <$(INZIG) > $(IFJCODE)
	./$(INTERPRETER) $(IFJCODE)

# Phony targets
.PHONY: all clean test run_tests run
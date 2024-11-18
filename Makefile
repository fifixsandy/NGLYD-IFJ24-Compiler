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
INTERETER := ic24int

# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -g

# Default target
all: $(NAME)

# Rule to build the executable
$(NAME): $(OBJFILES)
	mkdir -p $(OBJFOLDER)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@

# Rule to compile .c files into .o files
$(OBJFOLDER)/%.o: $(SRCFOLDER)/%.c
	mkdir -p $(OBJFOLDER)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the compiled files and the executable
clean:
	rm -rf $(OBJFOLDER)/
	rm -f $(NAME)
	rm -f $(TESTBINS)
	rm -f $(TESTFOLDER)/*.out

# Run tests
test: 
	./tests/test.sh ./tests $(NAME) $(INTERPRETER)

# Run the main executable

$(IFJCODE):
	touch $(IFJCODE)

$(INZIG):
	touch $(INZIG)

run: $(NAME)
	./$(NAME) <$(INZIG) > $(IFJCODE)
	./$(INTERETER) $(IFJCODE)

# Phony targets
.PHONY: all clean test run_tests run
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
TESTFILES := $(wildcard $(TESTFOLDER)/*/*.c)
TESTBINS := $(TESTFILES:$(TESTFOLDER)/%.c=$(TESTFOLDER)/%)


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
test: $(TESTBINS)

$(TESTBINS): $(TESTFILES) $(SRCFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Run the main executable
run: $(NAME)
	./$(NAME)

# Phony targets
.PHONY: all clean test run
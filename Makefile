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

IFJCODE := out.ifjcode

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


# Run all test executables
RED = \033[31m
GREEN = \033[32m
YELLOW = \033[33m
RESET = \033[0m

run_tests: $(TESTBINS)
	@for test in $^; do \
		path="$${test%/*}/"; \
		for input_file in $$path/test-*.in; do \
			output_file=$${input_file%.in}.out; \
			./$$test $$input_file $$output_file; \
			ref_file=$${input_file%.in}.ref; \
			if [ -f "$$ref_file" ]; then \
				if diff -q "$$output_file" "$$ref_file" > /dev/null; then \
					echo "$(GREEN)Test passed: $$input_file$(RESET)"; \
				else \
					echo "$(RED)Output differs from reference file!$(RESET)"; \
					echo "$$test $$input_file $$output_file "; \
					echo "$(YELLOW)Differences:${RESET}"; \
					diff "$$output_file" "$$ref_file"; \
				fi; \
			else \
				echo "$(YELLOW)No .ref file for test: $$input_file$(RESET)"; \
			fi; \
		done; \
	done

# Run the main executable
run: $(NAME) $(IFJCODE)
	touch $(IFJCODE)
	./$(NAME) > $(IFJCODE)
	./ic24int $(IFJCODE)

# Phony targets
.PHONY: all clean test run_tests run
#include "../../src/scanner.h"
#include <stdio.h>
#include <stdlib.h>

// Function to print the token to the specified file
void print_token_to_file(FILE *file, const Token *token) {
    if (file == NULL || token == NULL) {
        fprintf(stderr, "Error: Invalid file or token.\n");
        return;
    }

    fprintf(file, "%d", token->type);
    if (token->value != NULL) {
        fprintf(file, " %s\n", token->value);
    } else {
        fprintf(file, "\n");
    }
}

// Function to open or create a file
FILE *open_or_create_file(const char *filename) {
    FILE *file = fopen(filename, "w"); 
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open or create file '%s'.\n", filename);
        return NULL;
    }
    return file;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input_filename> <output_filename>\n", argv[0]);
        return 1; 
    }
    // Open the input file for reading
    input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
        return 1; 
    }

    // Open or create the output file
    FILE *output_file = open_or_create_file(argv[2]);
    if (output_file == NULL) {
        fclose(input_file);
        return 1;
    }

    // Process tokens and write them to the output file
    while (!feof(input_file)) {
        Token token = getToken();
        print_token_to_file(output_file, &token);
    }

    // Close the files when done
    fclose(input_file);
    fclose(output_file);
    return 0; 
}

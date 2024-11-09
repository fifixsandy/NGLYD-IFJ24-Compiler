/**
 * @file   code_buffer.h
 * @brief  Header file for managing a dynamic buffer that stores and manipulates strings and other data types such as integers and floats.
 *         The buffer supports dynamic expansion and safe data pushing.
 * 
 * @author xskovaj00
 * @date   6.11.2024
 */

#ifndef CODE_BUFFER_H
#define CODE_BUFFER_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Define the Buffer structure
typedef struct {
    char *str;           // The string buffer
    size_t length; // Current length of the buffer
    size_t alloc_size; // Allocated size of the buffer
} Buffer;

// Function prototypes for buffer operations
bool buf_init(Buffer **buf);
bool buf_expand(Buffer *buf, size_t min_add_size);
bool buf_push_code(Buffer *buf, char *str);
bool buf_push_int(Buffer *buf, int num);
bool buf_push_float(Buffer *buf, float num);
bool buf_push_string(Buffer *buf, char *str);
bool fprint_buffer(Buffer *buf, FILE *stream);

#endif // CODE_BUFFER_H

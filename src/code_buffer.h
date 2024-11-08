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

// Define the Buffer structure
typedef struct {
    char *str;           // The string buffer
    unsigned int length; // Current length of the buffer
    unsigned int alloc_size; // Allocated size of the buffer
} Buffer;

// Function prototypes for buffer operations
bool buf_init(Buffer *buf);
bool buf_expand(Buffer *buf);
bool buf_push_code(Buffer *buf, char *str);
bool buf_push_int(Buffer *buf, int num);
bool buf_push_float(Buffer *buf, float num);
bool buf_push_string(Buffer *buf, int *str);

#endif // CODE_BUFFER_H

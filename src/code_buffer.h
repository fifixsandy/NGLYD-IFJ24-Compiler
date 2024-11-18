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

//#define DEBUG
#ifdef DEBUG
    #ifndef DEBPRINT 
    #define DEBPRINT(...) \
        fprintf(stderr, "D: %s, %d: ", __func__ , __LINE__); \
        fprintf(stderr, __VA_ARGS__);
    #endif
#else
    #ifndef DEBPRINT 
    #define DEBPRINT(...) 
    #endif
#endif


typedef struct Buffer_node{
    char *str;
    struct Buffer_node *next;
} Buffer_node;

typedef struct {
    Buffer_node *first;
    Buffer_node *last;
    Buffer_node *flag;
    char *tmp;
} Buffer_ll;


// Function prototypes for buffer operations
bool buf_init(Buffer_ll **buf);
Buffer_node *buf_new_node();
bool buf_add(Buffer_ll *buf, char *str);
bool buf_push(Buffer_ll *buf);
bool buf_add_push(Buffer_ll *buf, char *str);
void buf_add_flag(Buffer_ll *buf);
void buf_delete_flag(Buffer_ll *buf);
bool buf_add_int(Buffer_ll *buf, int num);
bool buf_add_float(Buffer_ll *buf, float num);
bool buf_add_string(Buffer_ll *buf, char *str);
bool buf_push_after_flag(Buffer_ll *buf);
bool fprint_buffer(Buffer_ll *buf, FILE *stream);

#endif // CODE_BUFFER_H

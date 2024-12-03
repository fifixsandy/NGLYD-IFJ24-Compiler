/**
 *         Implementation of IFJ24 imperative language compiler.
 * 
 * @file   code_buffer.h
 * @brief  Header file for managing a dynamic buffer that stores stirngs.
 *         The buffer supports dynamic expansion and safe data pushing.
 *         Also support to add Flag to node, and store contentet after flagged node.
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

/**
 * @struct Buffer_node
 * @brief  Represents a single node in the linked list buffer.
 * 
 * Each node contains a dynamically allocated string and a pointer to the next node.
 */
typedef struct Buffer_node{
    char *str;  ///< Pointer to the stored string
    struct Buffer_node *next;   ///< Pointer to the next node in the list
} Buffer_node;


/**
 * @struct Buffer_ll
 * @brief  Represents the linked list buffer structure.
 * 
 */
typedef struct {
    Buffer_node *first; ///< Pointer to the first node in the list
    Buffer_node *last;  ///< Pointer to the last node in the list
    Buffer_node *flag;  ///< Pointer to a flagged node
    char *tmp;           ///< Accumulator string
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
bool buf_add_float(Buffer_ll *buf, double num);
bool buf_add_string(Buffer_ll *buf, char *str);
bool buf_push_after_flag(Buffer_ll *buf);
bool fprint_buffer(Buffer_ll *buf, FILE *stream);

#endif // CODE_BUFFER_H

/* EOF code_buffer.h */
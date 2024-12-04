/**
 *         Implementation of IFJ24 imperative language compiler.     
 * 
 * @file   code_buffer.c
 * @brief  Header file for managing a dynamic buffer that stores stirngs.
 *         The buffer supports dynamic expansion and safe data pushing.
 *         Also support to add Flag to node, and store contentet after flagged node.
 * 
 * @author xskovaj00
 * @date   6.11.2024
 */
#include "code_buffer.h"


/**
 * @brief Initializes the buffer linked list.
 * 
 * @param buf Pointer to the buffer linked list to be initialized.
 * @return true if initialization was successful, false otherwise.
 */
bool buf_init(Buffer_ll **buf){
    *buf = malloc(sizeof(Buffer_ll));
    if (*buf == NULL) {
        return false;
    }
    (*buf)->first = NULL;
    (*buf)->last = NULL;
    (*buf)-> flag = NULL;
    (*buf)->tmp = NULL;
    return true;
}


/**
 * @brief Creates a new node for the buffer linked list.
 * 
 * @return Pointer to the newly created node, or NULL if memory allocation fails.
 */
Buffer_node *buf_new_node(){
    Buffer_node *new = malloc(sizeof(Buffer_node));
    if(new == NULL){
        return NULL;
    }
    new->next = NULL;
    new->str = NULL;
    return new;
}


/**
 * @brief Adds a string to the accumulator string.
 * 
 * @param buf Pointer to the buffer linked list.
 * @param str String to be added to the accumulator string.
 * @return true if the string was successfully added, false otherwise.
 */
bool buf_add(Buffer_ll *buf, char *str){
    size_t old_len = (buf->tmp != NULL) ? strlen(buf->tmp) : 0;
    char *tmp = realloc(buf->tmp, sizeof(char)*(strlen(str)+old_len+1));
    if(tmp == NULL){
        return false;
    }
    if(buf->tmp == NULL){
        buf->tmp = tmp;
        strcpy(buf->tmp, str);
    }
    else{
        buf->tmp = tmp;
        strcat(buf->tmp, str);
    }
    return true;
}


/**
 * @brief Pushes the accumulator string content into a new node of the linked list.
 * 
 * @param buf Pointer to the buffer linked list.
 * @return true if the content was successfully pushed, false otherwise.
 */
bool buf_push(Buffer_ll *buf){
    Buffer_node *new = buf_new_node();
    if(new == NULL){
        return false;
    }
    if(buf->first == NULL){
        buf->first = new;
    }
    else{
        buf->last->next = new;
    }
    buf->last = new;
    new->str = buf->tmp;
    buf->tmp = NULL;
    return true;
}


/**
 * @brief Adds a string to the accumulator string and pushes it into the linked list.
 * 
 * @param buf Pointer to the buffer linked list.
 * @param str String to be added and pushed.
 * @return true if the operation was successful, false otherwise.
 */
bool buf_add_push(Buffer_ll *buf, char *str){
    if(!buf_add(buf, str)) return false;
    if(!buf_push(buf)) return false;
    return true;
}



/**
 * @brief Sets a flag to mark a specific node in the linked list.
 * 
 * @param buf Pointer to the buffer linked list.
 */
void buf_add_flag(Buffer_ll *buf){
    buf->flag = buf->last;
}


/**
 * @brief Deletes the flag from the buffer linked list.
 * 
 * @param buf Pointer to the buffer linked list.
 */
void buf_delete_flag(Buffer_ll *buf){
    buf->flag = NULL;
}


/**
 * @brief Adds an integer as a string to the accumulator string.
 * 
 * @param buf Pointer to the buffer linked list.
 * @param num Integer to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool buf_add_int(Buffer_ll *buf, int num){
    char str[30];  
    sprintf(str, "%d", num);
    if(!buf_add(buf, str)) return false;
    return true;
}


/**
 * @brief Adds a floating-point number (64 bit, floating point) as a string to the accumulator string.
 * 
 * @param buf Pointer to the buffer linked list.
 * @param num Floating-point number to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool buf_add_float(Buffer_ll *buf, double num){
    char str[100];  
    sprintf(str, "%a", num);
    if(!buf_add(buf, str)) return false;
    return true;
}


/**
 * @brief Adds a string  Adds a string with escaped characters to the accumulator string.
 * 
 * @param buf Pointer to the buffer linked list.
 * @param str String to be added.
 * @return true if the operation was successful, false otherwise.
 */
bool buf_add_string(Buffer_ll *buf, char *str){
    char tmp[5];
    for(size_t i = 0; i < strlen(str); i++){
        if(str[i] <= 32 || str[i] == 35 || str[i] == 92){
            sprintf(tmp, "\\%03d", str[i]);
            if(!buf_add(buf, tmp)) return false;
        }
        else{
            sprintf(tmp, "%c", str[i]);
            if(!buf_add(buf, tmp)) return false;
        }
    }
    return true;
}


/**
 * @brief Pushes the accumulator string content into a new node after the flagged node.
 * 
 * @param buf Pointer to the buffer linked list.
 * @return true if the operation was successful, false otherwise.
 */
bool buf_push_after_flag(Buffer_ll *buf){
    if(buf->flag == NULL) return false;
    Buffer_node *new = buf_new_node();
    if(new == NULL){
        return false;
    }
    new->str = buf->tmp;
    buf->tmp = NULL;
    
    if(buf->flag == buf->last){
        buf->last = new;
    }

    new->next = buf->flag->next;
    buf->flag->next = new;
    return true;
}


/**
 * @brief Prints the content of the buffer linked list to a file stream.
 * 
 * @param buf Pointer to the buffer linked list.
 * @param stream File stream where the content will be printed.
 * @return true if the operation was successful, false otherwise.
 */
bool fprint_buffer(Buffer_ll *buf, FILE *stream) {
    if (buf == NULL || stream == NULL) {
        return false;
    }
    Buffer_node *tmp = buf->first;
    while(tmp != NULL){
        if(tmp->str != NULL) fprintf(stream, "%s", tmp->str);
        tmp = tmp->next;
    }
    return true;
}

/* EOF code_buffer.c */
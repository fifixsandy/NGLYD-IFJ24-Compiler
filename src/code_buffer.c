/**
 * @file   code_buffer.c
 * @brief  This file contains functions to manage a dynamic buffer for storing and manipulating strings and other types of data.
 *          The buffer supports dynamic expansion and safe data pushing.
 * 
 * @author xskovaj00
 * @date   6.11.2024
 */
#include "code_buffer.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define INIT_SIZE_OF_BUFFER 1000

typedef struct
{
	char *str; 
	unsigned int length; 
	unsigned int alloc_size; 
} Buffer;

bool buf_init(Buffer *buf){
    buf->str = malloc(sizeof(char)*INIT_SIZE_OF_BUFFER);
    if(buf->str == NULL){
        return false;
    }
    *(buf->str) = "\0";
    buf->length = 1;
    buf->alloc_size = INIT_SIZE_OF_BUFFER;
}

bool buf_expand(Buffer *buf){
    buf->str = (char*) realloc(buf->str, buf->alloc_size*2);
    if(buf->str == NULL){
        return false;
    }
    buf->alloc_size = buf->alloc_size*2;
}

bool buf_push_code(Buffer *buf, char *str){
    if(buf->length + strlen(str) > buf->alloc_size){
        if(!buf_expand(buf)) return false;
    }
    strcat(buf->str, str);
    return true;
}

bool buf_push_int(Buffer *buf, int num){
    char str[30];  
    sprintf(str, "%d", num);
    if(!buf_push_code(buf, &str)) return false;
    return true;
}

bool buf_push_float(Buffer *buf, float num){
    char str[100];  
    sprintf(str, "%a", num);
    if(!buf_push_code(buf, &str)) return false;
    return true;
}

bool buf_push_string(Buffer *buf, int *str){
    char tmp[5];
    for(int i = 0; i < strlen(str); i++){
        if(str[i] <= 32 || str[i] == 35 || str[i] == 92){
            sprintf(&tmp, "\\%03d", str[i]);
            if(!buf_push_code(buf, &tmp)) return false;
        }
        else{
            sprintf(tmp, "%c", str[i]);
            if(!buf_push_code(buf, &tmp)) return false;
        }
    }

    return true;
}
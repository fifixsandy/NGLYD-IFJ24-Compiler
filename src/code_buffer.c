/**
 * @file   code_buffer.c
 * @brief  This file contains functions to manage a dynamic buffer for storing and manipulating strings and other types of data.
 *          The buffer supports dynamic expansion and safe data pushing.
 * 
 * @author xskovaj00
 * @date   6.11.2024
 */
#include "code_buffer.h"

#define INIT_SIZE_OF_BUFFER 11

bool buf_init(Buffer **buf){
    *buf = malloc(sizeof(Buffer));
    if (*buf == NULL) {
        return false;
    }
    (*buf)->str = malloc(sizeof(char)*INIT_SIZE_OF_BUFFER);
    if((*buf)->str == NULL){
        return false;
    }
    (*buf)->str[0] = '\0';
    (*buf)->length = 0;
    (*buf)->alloc_size = INIT_SIZE_OF_BUFFER-1;
    return true;
}

bool buf_expand(Buffer *buf, size_t min_add_size){
    size_t next_size = buf->alloc_size * 2 + min_add_size;
    char *tmp = realloc(buf->str, next_size * sizeof(char));
    if(tmp == NULL){
        return false;
    }
    buf->str = tmp;
    buf->alloc_size = next_size;
    return true;
}

bool buf_push_code(Buffer *buf, char *str){
    size_t len_str = strlen(str);
    if(buf->length + len_str > buf->alloc_size){
        if(!buf_expand(buf, len_str)) return false;
    }
    strcat(buf->str, str);
    buf->length += len_str;
    return true;
}

bool buf_push_int(Buffer *buf, int num){
    char str[30];  
    sprintf(str, "%d", num);
    if(!buf_push_code(buf, str)) return false;
    return true;
}

bool buf_push_float(Buffer *buf, float num){
    char str[100];  
    sprintf(str, "%a", num);
    if(!buf_push_code(buf, str)) return false;
    return true;
}

bool buf_push_string(Buffer *buf, char *str){
    char tmp[5];
    for(size_t i = 0; i < strlen(str); i++){
        if(str[i] <= 32 || str[i] == 35 || str[i] == 92){
            sprintf(tmp, "\\%03d", str[i]);
            if(!buf_push_code(buf, tmp)) return false;
        }
        else{
            sprintf(tmp, "%c", str[i]);
            if(!buf_push_code(buf, tmp)) return false;
        }
    }

    return true;
}

bool fprint_buffer(Buffer *buf, FILE *stream) {
    if (buf == NULL || buf->str == NULL || stream == NULL) {
        return false;
    }
    fprintf(stream, "%s", buf->str);
    return true;
}
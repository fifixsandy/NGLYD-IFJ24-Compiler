/**
 * @file   code_buffer.c
 * @brief  This file contains functions to manage a dynamic buffer for storing and manipulating strings and other types of data.
 *          The buffer supports dynamic expansion and safe data pushing.
 * 
 * @author xskovaj00
 * @date   6.11.2024
 */
#include "code_buffer.h"

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

Buffer_node *buf_new_node(){
    Buffer_node *new = malloc(sizeof(Buffer_node));
    if(new == NULL){
        return NULL;
    }
    new->next = NULL;
    new->str = NULL;
    return new;
}

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
    //DEBPRINT("push: %s\n", buf->tmp);
    buf->last = new;
    new->str = buf->tmp;
    buf->tmp = NULL;
    return true;
}

bool buf_add_push(Buffer_ll *buf, char *str){
    if(!buf_add(buf, str)) return false;
    if(!buf_push(buf)) return false;
    return true;
}


void buf_add_flag(Buffer_ll *buf){
    buf->flag = buf->last;
}

void buf_delete_flag(Buffer_ll *buf){
    buf->flag = NULL;
}

bool buf_add_int(Buffer_ll *buf, int num){
    char str[30];  
    sprintf(str, "%d", num);
    if(!buf_add(buf, str)) return false;
    return true;
}

bool buf_add_float(Buffer_ll *buf, double num){
    char str[100];  
    sprintf(str, "%a", num);
    if(!buf_add(buf, str)) return false;
    return true;
}

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

bool buf_push_after_flag(Buffer_ll *buf){
    if(buf->flag == NULL) return false;
    Buffer_node *new = buf_new_node();
    if(new == NULL){
        return false;
    }
    DEBPRINT("after: %s\n", buf->tmp);
    new->str = buf->tmp;
    buf->tmp = NULL;
    
    if(buf->flag == buf->last){
        buf->last = new;
    }

    new->next = buf->flag->next;
    buf->flag->next = new;
    return true;
}

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

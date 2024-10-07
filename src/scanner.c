#include "scanner.h"

FILE *input_file;

token_types is_next_token(FILE *file, Token *token, char expected_char, token_types type1,token_types type2){
    char nextchar = getc(file);
    if(nextchar == expected_char) {
                token->type = type2;
                return type2;
            }
            else {
                ungetc(nextchar, file);
                token->type = type1;
                return type1;
            }
}

int init_value(char **buffer, size_t initial_size) {
    *buffer = malloc(initial_size);
    if (*buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return -1;
    }
    return 0;
}

int realloc_value(char **buffer, size_t *buffer_size) {
    *buffer_size *= 2;
    *buffer = realloc(*buffer, *buffer_size);
    if (*buffer == NULL) {
        fprintf(stderr, "Failed to reallocate memory\n");
        return -1;
    }
    return 0;
}

Token getToken() {
    char c;
    char nextchar;
    char expected_char;
    
    Token current_token;
    current_token.value = NULL;
    current_token.type = tokentype_invalid;
    
    while((c = getc(input_file)) == ' ' || c == '\n' || c == '\t') {
        if(c == '\n') {
            current_token.type == tokentype_EOL;
        }
        else if(c == EOF) {
            current_token.type == tokentype_EOF;
        }
    }

    switch(c) {
        case '.':
            current_token.type = tokentype_dot;
            break;
        
        case ':':
            current_token.type = tokentype_colon;
            break;
        
        case '@':
            //TODO
            //import_function();
            break;
        
        case '|':
            current_token.type = tokentype_vbar;
            break;
        
        case ';':
            current_token.type = tokentype_semicolon;
            break;
        
        case '+':
            current_token.type = tokentype_plus;
            break;
        
        case '-':
            current_token.type = tokentype_minus;
            break;
        
        case '*':
            current_token.type = tokentype_multiply;
            break;
        
        case '/':
            expected_char = '/';
            is_next_token(input_file, &current_token, expected_char, tokentype_divide, tokentype_comment);
            break;
        
        case '=':
            expected_char = '=';
            is_next_token(input_file, &current_token, expected_char, tokentype_assign, tokentype_equal);
            break;

        case '<':
            expected_char = '=';
            is_next_token(input_file, &current_token, expected_char, tokentype_lower, tokentype_lowerequal);
            break;
        
        case '>':
            expected_char = '=';
            is_next_token(input_file, &current_token, expected_char, tokentype_greater, tokentype_greaterequal);
            break;
        
        case '(':
            current_token.type = tokentype_lbracket;
            break;
        
        case ')':
            current_token.type = tokentype_rbracket;
            break;
        
        case '{':
            current_token.type = tokentype_lcbracket;
            break;

        case '}':
            current_token.type = tokentype_rcbracket;

        case '!':
            nextchar = getc(input_file);
            if (nextchar == '=') {
                current_token.type = tokentype_notequal;
            }
            else {
                current_token.type = tokentype_invalid;
                return current_token;
            }
            break;
        
        case '0' ... '9':
            current_token = process_Number_Token(c, input_file);
            break;
        
        case '"':
            current_token = process_String_Token(c, input_file);
            break;

        default:
            current_token.type = tokentype_id;
    }
    return current_token;
}   

Token process_Number_Token(char firstchar, FILE *input_file) {
        
    Token current_token;
    char nextchar;
    size_t buffer_size = 2;
    int index = 0;
        
    if(init_value(&current_token.value, buffer_size) == -1) {
        current_token.type = tokentype_invalid;
        return current_token;
    }

    current_token.value[index++] = firstchar;

    if(firstchar == '0') {
        if(isdigit(nextchar = getc(input_file))) {
            current_token.type = tokentype_invalid;
            fprintf(stderr, "A whole number cannot start with 0\n");
            //ungetc(nextchar, input_file); DO OR DONT?
            return current_token;
        }
        current_token.type = tokentype_zeroint;
        nextchar = getc(input_file);

    }
    else {
        current_token.type = tokentype_int;
        
        while(isdigit((nextchar = getc(input_file)))) {
            if (index >= buffer_size - 1) {
                if (realloc_value(&current_token.value, &buffer_size) == -1) {
                    current_token.type = tokentype_invalid;
                    return current_token;
                }
            }
            current_token.value[index++] = (char) nextchar;
        }
    }
    
    if (nextchar == '.') {
        current_token.type = tokentype_float;
        current_token.value[index++] = (char) nextchar;
            
        while(isdigit((nextchar = getc(input_file)))) {
            if (index >= buffer_size - 1) {
                if (realloc_value(&current_token.value, &buffer_size) == -1) {
                    current_token.type = tokentype_invalid;
                    return current_token;
                }
            }
            current_token.value[index++] = (char) nextchar;
        }
    }
    if(nextchar == 'e' || nextchar == 'E') {
            
        if(current_token.type == tokentype_zeroint) {
            fprintf(stderr, "Number zero cannot have an exponent\n");
            current_token.type = tokentype_invalid;
            return current_token;
        }
        current_token.type = tokentype_exponentialnum;
        current_token.value[index++] = (char) nextchar;

        nextchar = getc(input_file);
        if(nextchar == '+' || nextchar == '-') {
            current_token.value[index++] = (char) nextchar;
            nextchar = getc(input_file);
        }

        while(isdigit(nextchar)) {
            if (index >= buffer_size - 1) {
                if (realloc_value(&current_token.value, &buffer_size) == -1) {
                    current_token.type = tokentype_invalid;
                    return current_token;
                }
            }
            current_token.value[index++] = (char) nextchar;
            nextchar = getc(input_file);
        }
    }
    if(current_token.value[index-1] == 'e' || 
        current_token.value[index-1] == 'E' || 
        current_token.value[index-1] == '.' ||
        current_token.value[index-1] == '-' ||
        current_token.value[index-1] == '+' )
        {
        fprintf(stderr, "Number incomplete\n");
        current_token.type = tokentype_invalid;
        return current_token;
    }       
    if(!isdigit(nextchar)) {
        ungetc(nextchar, input_file);
    }
    current_token.value[index] = '\0';
    printf("%s\n", current_token.value);
    printf("%d\n", current_token.type);
}


Token process_String_Token(char firstchar, FILE *input_file) {
        
    Token current_token;
    char nextchar;
    size_t buffer_size = 2;
    int index = 0;

    current_token.type = tokentype_string;

    if(init_value(&current_token.value, buffer_size) == -1) {
        current_token.type = tokentype_invalid;
        return current_token;
    }
    current_token.value[index++] = firstchar;
        
    while((nextchar = getc(input_file)) != '"' && nextchar != 92) {
        if (index >= buffer_size - 1) {
            if (realloc_value(&current_token.value, &buffer_size) == -1) {
                current_token.type = tokentype_invalid;
                return current_token;
            }
        }
        current_token.value[index++] = nextchar;

        if(nextchar == '\n' || nextchar == EOF) {
            fprintf(stderr, "String incorrect\n");
            current_token.type = tokentype_invalid;
            return current_token;
        }

    }
    if(nextchar == '"') {
        current_token.value[index++] = nextchar;
    }
    else if(nextchar == 92) {
        current_token.value[index++] = nextchar;
            
        nextchar = getc(input_file);
        if((nextchar == 'n') || 
            nextchar == 'r' || 
            nextchar == 't' || 
            nextchar == '"' || 
            nextchar == 92) 
        {
            current_token.value[index++] = nextchar;
                
            nextchar = getc(input_file);
            if(nextchar == '"') {
                current_token.value[index++] = nextchar;
            }
            else{
                fprintf(stderr, "Escape sequence incorrect\n");
                current_token.type = tokentype_invalid;
                return current_token;
            }
        }
        else if(nextchar == 'x') {
            current_token.value[index++] = nextchar;
                
            for(int i = 0; i < 2; i++) {
                if(((nextchar = getc(input_file)) >= '0' && nextchar <= '9') ||
                    (nextchar >= 'a' && nextchar <= 'f') || 
                    (nextchar >= 'A' && nextchar <= 'F'))    
                {
                current_token.value[index++] = nextchar;
                }
                else {
                    fprintf(stderr, "Hexadecimal number incorrect\n");
                    current_token.type = tokentype_invalid;
                    return current_token;
                }
            }
            if((nextchar = getc(input_file)) == '"') {
                current_token.value[index++] = nextchar;
            }
            else {
                fprintf(stderr, "Hexadecimal number incorrect\n");
                current_token.type = tokentype_invalid;
                return current_token;
            }
        }
        else {
            fprintf(stderr, "Escape sequence incorrect\n");
            current_token.type = tokentype_invalid;
            return current_token;
        }
    }
    else {
        fprintf(stderr, "String incomplete\n");
        current_token.type = tokentype_invalid;
        return current_token;
    }
    current_token.value[index] = '\0';
    printf("%s\n", current_token.value);
    printf("%d\n", current_token.type);
    }

int main() {

    input_file = fopen("file.txt", "r");
    getToken();
    getToken();
}
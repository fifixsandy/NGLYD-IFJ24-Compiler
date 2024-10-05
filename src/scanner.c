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

int getToken() {
    char c;
    char nextchar;
    int state;
    Token current_token;
    char expected_char;
    
    while((c = getc(input_file)) == ' ' || c == '\n' || c == '\t') {
        if(c == '\n') {
            current_token.type == tokentype_EOL;
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
        
        //case 'EOL':
            //current_token.type = tokentype_EOL;
            //break;
        //case 'EOF':
            //current_token.type = tokentype_EOF;
            //break;
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
                ungetc(nextchar, input_file);
                //TODO ERROR
            }
            break;
        case '0':
            current_token.type = tokentype_zeroint;
            break;
        case '1' ... '9':
            current_token.type = tokentype_int;
            break;
        case '"':
            current_token.type = tokentype_string;
    }

    if(current_token.type == tokentype_zeroint || current_token.type == tokentype_int) {
        
        size_t buffer_size = 2;
        if(init_value(&current_token.value, buffer_size) == -1) {
            return -1;
        }

        int index = 0;
        current_token.value[index++] = c;
        
        while(isdigit((nextchar = getc(input_file)))) {
            if (index >= buffer_size - 1) {
                if (realloc_value(&current_token.value, &buffer_size) == -1) {
                    return -1;
                }
            }

            current_token.value[index++] = (char) nextchar;
        }

        //current_token->value[index] = '\0' TREBA NETREBA? POVEDZTE CHAT
        if (nextchar == '.') {
            current_token.type = tokentype_float;
            current_token.value[index++] = (char) nextchar;
            
            while(isdigit((nextchar = getc(input_file)))) {
                if (index >= buffer_size - 1) {
                    if (realloc_value(&current_token.value, &buffer_size) == -1) {
                        return -1;
                    }
                }
                current_token.value[index++] = (char) nextchar;
            }
        }
        if(nextchar == 'e' || nextchar == 'E') {
            if(current_token.type == tokentype_zeroint) {
                fprintf(stderr, "Number zero cannot have an exponent\n");
                return -1;
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
                        return -1;
                    }
                }
                current_token.value[index++] = (char) nextchar;
                nextchar = getc(input_file);
            }
        }
        if(current_token.value[index-1] == 'e' || current_token.value[index-1] == 'E' || current_token.value[index-1] == '.') {
            fprintf(stderr, "Number incomplete\n");
            return -1;
        }       
        if(!isdigit(nextchar)) {
            ungetc(nextchar, input_file);
        }
    }
    
    else if(current_token.type == tokentype_string) {
        
        size_t buffer_size = 2;
        if(init_value(&current_token.value, buffer_size) == -1) {
            return -1;
        }

        int index = 0;
        current_token.value[index++] = c;
        char nextchar;
        
        while((nextchar = getc(input_file)) != '"' && nextchar != 92) {
            if (index >= buffer_size - 1) {
                if (realloc_value(&current_token.value, &buffer_size) == -1) {
                    return -1;
                }
            }

            current_token.value[index++] = nextchar;
        }
        current_token.value[index++] = nextchar;
    }

    printf("%s\n", current_token.value); //POMOCNE VYPISY
    printf("%d\n", current_token.type);
    return 0;
}   

int main() {

    input_file = fopen("file.txt", "r");
    getToken();
    getToken();
    getToken();
    getToken();
}
#include "scanner.h"

const char *keywords[NUM_OF_KEYWORDS] = {
    "const", "else", "fn", "if", "i32", "f64", "null", 
    "pub", "return", "u8", "var", "void", "while"
};

FILE *input_file;
int Line_Number = 1;

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

int is_keyword(const char *str, Token *token) {
    for(int i = 0; i < NUM_OF_KEYWORDS; i++) {
        if(strcmp(str, keywords[i]) == 0) {
            int index = i + FIRST_KEYWORD;
            token->type = (token_types)index;
            return 1;
        }
    }
    return 0;
}

int init_value(char **buffer, int initial_size) {
    *buffer = malloc(initial_size);
    if (*buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return -1;
    }
    return 0;
}

int realloc_value(char **buffer, int *buffer_size) {
    *buffer_size *= 2;
    *buffer = realloc(*buffer, *buffer_size);
    if (*buffer == NULL) {
        fprintf(stderr, "Failed to reallocate memory\n");
        return -1;
    }
    return 0;
}

int cleanup_value(Token *token) {
    if (token == NULL) {
        return -1;  
    }

    if (token->value != NULL) {
        free(token->value);  
        token->value = NULL;
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
    
    here:
    while((c = getc(input_file)) == ' ' || c == '\t' || c == '\n') {
        if(c == '\n') {
            Line_Number++;;
        }
    }
    if(c == EOF) {
        current_token.type = tokentype_EOF;
        current_token.line = Line_Number;
        return current_token;
    }
    
    switch(c) {
        case '/':
            if((c = getc(input_file)) == '/') {
                while(c != '\n') { 
                    c = getc(input_file);
                    continue;
                }
                Line_Number++;
                goto here;
            }
            else {
                ungetc(c, input_file);
                current_token.type = tokentype_divide;
                break;
            }
                
        case '.':
            current_token.type = tokentype_dot;
            break;
        
        case ':':
            current_token.type = tokentype_colon;
            break;
        
        case '@':
            current_token = process_Import(input_file);
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
            break;

        case '?':
            current_token.type = tokentype_nullid;
            break;

        case '!':
            nextchar = getc(input_file);
            if (nextchar == '=') {
                current_token.type = tokentype_notequal;
            }
            else {
                ungetc(nextchar, input_file);
                current_token.type = tokentype_invalid;
            }
            break;
        
        case '"':
            current_token = process_String_Token(c, input_file);
            break;

        case '[':
            current_token.type = tokentype_lsbracket;
            break;

        case ']':
            current_token.type = tokentype_rsbracket;
            break;
        
        case ',' :
            current_token.type = tokentype_comma;
            break;
        
        case '\\':
            current_token = process_Multiline_String_Token(input_file);
            break;

        default:
            if(c >= '0' && c <= '9') {
                current_token = process_Number_Token(c, input_file);
            }
            else if (c == '_') {
                nextchar = getc(input_file);
                if(!isalnum(nextchar) && nextchar != '_') {
                    current_token.type = tokentype_pseudovar;
                    ungetc(nextchar, input_file);

                    //printf("%d\n", current_token.type);
                }
                else {
                    ungetc(nextchar, input_file);
                    current_token = process_ID_Token(c, input_file);
                }
            }
            else {
                current_token = process_ID_Token(c, input_file);
            }    
    }
    current_token.line = Line_Number;
    return current_token;
}   

Token process_Number_Token(char firstchar, FILE *input_file) {
        
    Token current_token;
    char nextchar;
    int buffer_size = 2;
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
            ungetc(nextchar, input_file);
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
    //printf("%s\n", current_token.value);
    //printf("%d\n", current_token.type);
    return current_token;
}


Token process_String_Token(char firstchar, FILE *input_file) {
        
    Token current_token;
    char nextchar;
    int buffer_size = 2;
    int index = 0;

    current_token.type = tokentype_string;

    if(init_value(&current_token.value, buffer_size) == -1) {
        current_token.type = tokentype_invalid;
        return current_token;
    }
        
    while((nextchar = getc(input_file)) != '"' && nextchar != '\n') {
        if (index >= buffer_size - 1) {
            if (realloc_value(&current_token.value, &buffer_size) == -1) {
                current_token.type = tokentype_invalid;
                return current_token;
            }
        }
        if(nextchar == EOF) {
            fprintf(stderr, "String incorrect\n");
            current_token.type = tokentype_invalid;
            return current_token;
        }

        if(nextchar == 92) {
            
            nextchar = getc(input_file);
            if(nextchar == 'n') {
                current_token.value[index++] = '\n';
            }
            else if(nextchar == 'r') {
                current_token.value[index++] = '\r';
            }  
            else if(nextchar == 't') {
                current_token.value[index++] = '\t';
            }
            else if(nextchar == '"') {
                current_token.value[index++] = '"';
            }
            else if(nextchar == 92) {
                current_token.value[index++] = '\\';   
            }
            
            else if(nextchar == 'x') {
                char hex_str[3] = {0};
                
                for(int i = 0; i < 2; i++) {
                    if(((nextchar = getc(input_file)) >= '0' && nextchar <= '9') ||
                        (nextchar >= 'a' && nextchar <= 'f') || 
                        (nextchar >= 'A' && nextchar <= 'F'))    
                    {
                    hex_str[i] = nextchar;
                    }
                    else {
                        fprintf(stderr, "Hexadecimal number incorrect\n");
                        current_token.type = tokentype_invalid;
                        return current_token;
                    }
                }
                long dec_value = strtol(hex_str, NULL, 16);
                sprintf(current_token.value + index, "%ld", dec_value);
                index += strlen(current_token.value + index);
            }
            else {
                fprintf(stderr, "Escape sequence incorrect\n");
                current_token.type = tokentype_invalid;
                return current_token;
            }
        }
        else {
            current_token.value[index++] = nextchar;
        }
    }
    if(nextchar == '\n') {
        current_token.type = tokentype_invalid;
        Line_Number++;
        return current_token;
    }

    current_token.value[index] = '\0';
    //printf("%s\n", current_token.value);
    //printf("%d\n", current_token.type);

    return current_token;
}



Token process_ID_Token(char firstchar, FILE *input_file) {
    
    Token current_token;
    char nextchar;
    int buffer_size = 2;
    int index = 0;

    if(init_value(&current_token.value, buffer_size) == -1) {
        current_token.type = tokentype_invalid;
        return current_token;
    }
    
    if(isalpha(firstchar) || firstchar == '_') {
        current_token.type = tokentype_id;
    }

    else {
        current_token.type = tokentype_invalid;
        return current_token;
    }
    
    current_token.value[index++] = firstchar;

    while((isalpha(nextchar = getc(input_file))) || isdigit(nextchar) || nextchar == '_') {
        if (index >= buffer_size - 1) {
            if (realloc_value(&current_token.value, &buffer_size) == -1) {
                current_token.type = tokentype_invalid;
                return current_token;
            }
        }
        current_token.value[index++] = nextchar;
    }
    current_token.value[index] = '\0';

    ungetc(nextchar, input_file);

    is_keyword(current_token.value, &current_token);


    //printf("%s\n", current_token.value);
    //printf("%d\n", current_token.type);

    return current_token;
}

/*Token process_Char_Arr(FILE *input_file) {

    Token current_token;
    char nextchar;
    current_token.type = tokentype_invalid;

    if((nextchar = getc(input_file)) == ']') {
        if((nextchar = getc(input_file)) == 'u') {
            if((nextchar = getc(input_file)) == '8') {
                current_token.type = tokentype_chararr;
            }
        }
    }
    //printf("%d\n", current_token.type);
    return current_token;
}*/

Token process_Import(FILE *input_file) {
    
    Token current_token;
    current_token.type = tokentype_invalid;

    const char *keyword = "import";
    char nextchar;
    int i = 0;

    while (keyword[i] != '\0') {
        nextchar = getc(input_file);

        if (nextchar != keyword[i]) {
            return current_token;  
        }
        i++; 
    }
    current_token.type = tokentype_import;
    
    //printf("%d\n", current_token.type);
    return current_token;
}

Token process_Multiline_String_Token(FILE *input_file) {
    char nextchar;
    Token current_token;
    int index = 0;
    int buffer_size = 2;

    if(init_value(&current_token.value, buffer_size) == -1) {
        current_token.type = tokentype_invalid;
        return current_token;
    }
    
    if((nextchar = getc(input_file)) == '\\') {
        current_token.type = tokentype_string;
        
        while(1) {
                
            if (index >= buffer_size - 1) {
                if (realloc_value(&current_token.value, &buffer_size) == -1) {
                    current_token.type = tokentype_invalid;
                    return current_token;
                }
            }
            nextchar = getc(input_file);
            
            if(nextchar == EOF) {
                break;
            }
            if(nextchar == '\n') {
                Line_Number++;
                char tempchar;
                while((tempchar = getc(input_file)) != EOF && isspace(tempchar)) {
                    if(tempchar == '\n') {
                        break;
                    }
                }
                if(tempchar == '\\' && (nextchar = getc(input_file)) == '\\') {
                    current_token.value[index++] = '\n';
                    continue;
                }
                else {
                    Line_Number--;
                    ungetc(tempchar, input_file);
                    ungetc(nextchar, input_file);
                    break;
                }
            }
            else {
                current_token.value[index++] = nextchar;
            }
        }
        current_token.value[index] = '\0';
    }
    else {
        current_token.type = tokentype_invalid;
        return current_token;
    }

    //printf("%s", current_token.value);
    //printf("%d\n", current_token.type);

    return current_token;
}

//TODO FUNCKCIA NA BUILTIN FUNKCIE

//  int main() {

//      input_file = fopen("file.txt", "r");
    
//      for(int i = 0; i < 10; i++) {
//          getToken();
//      }

//      return 0;
//  }
#include "scanner.h"

FILE *input_file;

token_types is_next_token(FILE *file, Token *token, char expected_char, token_types type1,token_types type2){
    char nextchar = getc(file);
    if(nextchar == expected_char) {
                token->type = type2;
                return type2
            }
            else {
                ungetc(nextchar, file);
                token->type = type1;
                return type1
            }
}

int getToken() {
    char c = getc(input_file);
    int state;
    Token current_token;
    
    switch(c) {
        case '.':
            current_token.type = tokentype_dot;
            break;
        case ':':
            current_token.type = tokentype_colon;
            break;
        
        case '@':
            import_function();
            break;
        
        case 'EOL':
            current_token.type = tokentype_EOL;
            break;
        case 'EOF':
            current_token.type = tokentype_EOF;
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
            char expected_char = '/';
            is_next_token(input_file, &current_token, expected_char, tokentype_divide, tokentype_comment);
            break;
        
        case '=':
            char expected_char = '=';
            is_next_token(input_file, &current_token, expected_char, tokentype_assign, tokentype_equal);
            break;

        case '<':
            char expected_char = '=';
            is_next_token(input_file, &current_token, expected_char, tokentype_lower, tokentype_lowerequal);
            break;
        
        case '>':
            char expected_char = '=';
            is_next_token(input_file, &current_token, expected_char, tokentype_greater, tokentype_greaterequal);
            break;
        
        case '(':
            current_token.type = tokentype_lbracket;
            break;
        case ')':
            current_token.type = tokentype_rbracket;
            break;
        
        case '!':
            nextchar = getchar(input_file);
            if (nextchar == '=') {
                current_token.type = tokentype_notequal;
            }
            else {
                ungetc(nextchar, input_file);
                //ERROR
            }
            break;

        case '0' ... '9':
            char randomchar = getchar();
            if (randomchar == ".") {
                //je to float
                //citaj dalej cislo
                //ESTE MOZE BYT AJ E
            }
            else if(randomchar == "e" || randomchar == "E") {
                //je to exponent
                //citaj dalej cislo, ZISTI  ZNAMIENKO
            }
            else {
                ungetc();
                //je to nula
            }
        case '1' ... '9':
            //citaj cele cislo FUNKCIA
            //ZISTI BODKU EČKO
        case '"':
            char randomc = getchar();
                if(randomc == "TU MUSI BYT BACKSLASH") {
                    //n, r, t, picoviny
                    // alebo hexadec
                }
                else if(randomc == ascii nejake) {
                    //citaj cely string
                }
    }
}   

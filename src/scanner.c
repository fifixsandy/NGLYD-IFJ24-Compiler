/**
 * @file   scanner.c
 * 
 * @brief  Implementation of scanner for IFJ24
 * 
 *         Scanner reads from standard input and processes
 *         input into tokens. Tokens hold information about
 *         their value, their line and their type. It also
 *         recognizes lexical errors and calls an error function
 *         when needed.
 *         
 * @author xmalegt00
 * @date   15.11.2024
*/

#include "scanner.h"

// This array of strings holds each keyword in order to be recognized as keywords
const char *keywords[NUM_OF_KEYWORDS] = {
    "const", "else", "fn", "if", "i32", "f64", "null", 
    "pub", "return", "u8", "var", "void", "while"
};

int Line_Number = 1;            //Initializing Line_Number to 1, because we are starting on the first line
int Column_Number = 0;          //Initializing Column_Numer to 0, because we havent yet read any character
TokenValues *head = NULL;       //Initializing the head of linked list holding values of tokens to NULL


/**
 * @brief Function which decides between token types based on the next character in input.
 * 
 * @param token         Token to which is the type assigned.
 * @param expected_char Character which is expected to follow.
 * @param type1         Return token type if condition is not met.
 * @param type2         Return token type if condition is met.
 * 
 * @return Returns type of token which is then assigned to token.
 * 
 */
token_types is_next_token(Token *token, char expected_char, token_types type1,token_types type2){
    char nextchar = getc(stdin);
    if(nextchar == expected_char) {
        Column_Number++;
        token->type = type2;
        return type2;
    }
    else {
        ungetc(nextchar, stdin);
        token->type = type1;
        return type1;
    }
}

/**
 * @brief Function which compares ID token and decides if its a keyword
 * 
 * @param str   String which we compare.
 * @param token Token to which we assign its type.
 * 
 * @return Zero if match found in keywords.
 */
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

/**
 * @brief Function for initializing an array holding value of token.
 * 
 * @param buffer        Pointer to a memory to be allocated.
 * @param initial_size  Size of how much memory to allocate.
 * 
 * @return Zero if alloc was successful.
 */
int init_value(char **buffer, int initial_size) {
    *buffer = malloc(initial_size);
    if (*buffer == NULL) {
        ERRORLEX(ERR_INTERNAL, "Failed to allocate memory for token value. Line: %d.\n", Line_Number);
    }
    return 0;
}

/**
 * @brief Function for reallocing value size if it exceeds half of the memory currently allocated.
 * 
 * @param buffer        Pointer to a memory to be allocated.
 * @param buffer_size   Size of how much memory to allocate.
 * 
 * @return Zero if reallocation was successful.
 */
int realloc_value(char **buffer, int *buffer_size) {
    *buffer_size *= 2;
    *buffer = realloc(*buffer, *buffer_size);
    if (*buffer == NULL) {
        ERRORLEX(ERR_INTERNAL, "Failed to reallocate memory for token value. Line: %d.\n", Line_Number);
    }
    return 0;
}

/**
 * @brief Function which adds the value of token into a linked list in order to be freed later.
 * 
 * @param value Holds the pointer to a memory containing value of token.
 * 
 */
void add_value_pointer(char *value) {
    TokenValues *new_node = (TokenValues *) malloc(sizeof(TokenValues));
    if(new_node == NULL) {
        ERRORLEX(ERR_INTERNAL, "Failed to allocate memory for TokenValues.\n");
    }
    new_node->value = value;
    new_node->next = head;
    head = new_node;

}

/**
 * @brief Function which frees all allocated memory for holding the value of tokens.
 */
void free_all_values() {
    TokenValues *current = head;
    while(current != NULL) {
        TokenValues *temp = current;
        free(current->value);
        current = current->next;
        free(temp);
    }
    head = NULL;
}

/**
 * @brief Function which resets the scanner to initial state, ready to read from input again.
 */
void reset_scanner() {
    rewind(stdin);
    Line_Number = 1;
    Column_Number = 0;
}

/**
 * @brief   Main scanner function which tokenizes the input.
 * 
 * @return  getToken functions returns a single token which contains its value,
 *          its type its line and column number
 * 
 */
Token getToken() {
    
    char c;                              //first character which will be read from input
    char nextchar;                       //next character which will bea read from input
    char expected_char;                  //expected char which will be used in a decision making function
    Token current_token;                 //token of which type and value we are processing
    current_token.value = NULL;          //initializing token value to NULL
    current_token.type = tokentype_EOF;  //setting the initial state of token type to EOF
    int FirstCharOfToken;                //declaring an integer for storing the index of first char of token
    
    here:
    while((c = getc(stdin)) == ' ' || c == '\t' || c == '\n') { //Skipping every white character
        Column_Number++;
        if(c == '\n') {             
            Line_Number++;         //incrementing line number when next line character was found
            Column_Number = 0;      //resetting column on new line
        }
    }

    Column_Number++;                       
    FirstCharOfToken = Column_Number;       //Saving the index of first char of token

    if(c == EOF) {                              //We reached the end of file
        current_token.type = tokentype_EOF;     //set the type to EOF
        current_token.line = Line_Number;       //and assign the line number 
        current_token.column = Column_Number;
        return current_token;
    }
    switch(c) {       //switch for making decisions based on the first character read
        case '/':
            if((c = getc(stdin)) == '/') {  //If the first two characters are "//"
                Column_Number++;
                while(c != '\n') {               //skip until you reach end of line.
                    if(c == EOF) {
                        current_token.type = tokentype_EOF;
                        current_token.line = Line_Number;
                        return current_token;
                    }
                    c = getc(stdin);
                    continue;
                }
                Line_Number++;                  //Increment Line number after reaching end of line
                Column_Number = 0;              //reset column when reaching new line
                goto here;                      //Jump to the beginning of getToken to start reading again.
            }       
            else {
                ungetc(c, stdin);                     //if its only a single "/", 
                current_token.type = tokentype_divide;     //assign token type.
                break;
            }
                
        case '.':
            current_token.type = tokentype_dot;
            break;
        
        case ':':
            current_token.type = tokentype_colon;
            break;
        
        case '@':
            current_token = process_Import();
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
            expected_char = '=';    //using is_next_token to assign type correctly
            is_next_token(&current_token, expected_char, tokentype_assign, tokentype_equal);
            break;

        case '<':
            expected_char = '=';
            is_next_token(&current_token, expected_char, tokentype_lower, tokentype_lowerequal);
            break;
        
        case '>':
            expected_char = '=';
            is_next_token(&current_token, expected_char, tokentype_greater, tokentype_greaterequal);
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
            nextchar = getc(stdin);
            if (nextchar == '=') {
                Column_Number++;
                current_token.type = tokentype_notequal;
            }
            else {
                ungetc(nextchar, stdin);
                ERRORLEX(ERR_LEX, "Invalid character on line %d, column %d.\n", Line_Number, Column_Number);
            }
            break;
        
        case '"':
            current_token = process_String_Token();
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
            current_token = process_Multiline_String_Token();
            break;

        default:                        //if none of conditions above were met, default will handle all the rest
            if(c >= '0' && c <= '9') {
                current_token = process_Number_Token(c);
            }

            else if (c == '_') {
                nextchar = getc(stdin);
                if(!isalnum(nextchar) && nextchar != '_') {     //Deciding if its a pseudovariable or an ID
                    current_token.type = tokentype_pseudovar;
                    ungetc(nextchar, stdin);
                }
                else {
                    ungetc(nextchar, stdin);
                    current_token = process_ID_Token(c);
                }
            }
            else {      //else branch for processing the only type left, ID type.
                current_token = process_ID_Token(c); 
            }    
    }
    if(current_token.value != NULL) {
        add_value_pointer(current_token.value); //Adding value to list of values.
    }

    current_token.line = Line_Number;           //Assigning line number to token.
    current_token.column = FirstCharOfToken;    //Assigning index of first char of token to token
    
    //printf("L: %d, C: %d\n", current_token.line, current_token.column);
    //printf("COLNUM: %d\n", Column_Number);
    //printf("%s\n", current_token.value);
    //printf("%d\n", current_token.type);

    return current_token;                       //Return processed token.
}   

/**
 * @brief  Function which processes number tokens.
 * 
 * @param firstchar   First character of currently processed token.
 * 
 * @return Returns processed token of number type.
 */
Token process_Number_Token(char firstchar) {
    Token current_token;
    char nextchar;
    int buffer_size = 2;    //initial buffer_size is 2, used for reallocating memory
    int index = 0;          //index of value is intitialized to zero

    init_value(&current_token.value, buffer_size);  //initializing value array

    current_token.value[index++] = firstchar;       //Adding the first character of token to value.

    //processing zero and whole part of a number
    if(firstchar == '0') {
        if(isdigit(nextchar = getc(stdin))) {
            ungetc(nextchar, stdin);
            ERRORLEX(ERR_LEX, "A whole number cannot start with 0. Line %d, column %d.\n", Line_Number, Column_Number);
        }
        else {
            current_token.type = tokentype_zeroint;     //assigning type zeroint if its a zero
        }
    }
    else {                                          //else process a non zero number
        current_token.type = tokentype_int;
        
        while(isdigit((nextchar = getc(stdin)))) {
            if (index >= buffer_size - 1) {               //reallocate memory if needed
                realloc_value(&current_token.value, &buffer_size);
            }
            current_token.value[index++] = (char) nextchar; //add another character to value
            Column_Number++;
        }
    }
    
    //processing the decimal part of a number or an exponent
    if (nextchar == '.') {         
        current_token.type = tokentype_float;
        current_token.value[index++] = (char) nextchar;
            
        while(isdigit((nextchar = getc(stdin)))) { //allocating more memory if needed
            if (index >= buffer_size - 1) {
                realloc_value(&current_token.value, &buffer_size);
            }
            current_token.value[index++] = (char) nextchar; //adding character to array
            Column_Number++;
        }
        Column_Number++;
    }
    if((nextchar == 'e' || nextchar == 'E') && current_token.value[index-1]!= '.') {
        if(current_token.type == tokentype_zeroint) {
            ERRORLEX(ERR_LEX, "Number zero cannot have an exponent. Line %d, column %d.\n", Line_Number, Column_Number);
        }
        current_token.type = tokentype_exponentialnum;
        current_token.value[index++] = (char) nextchar; //adding character to array

        nextchar = getc(stdin);
        Column_Number++;
        if(nextchar == '+' || nextchar == '-') {    //considering a sign of an exponential number
            current_token.value[index++] = (char) nextchar;
            nextchar = getc(stdin);
            Column_Number++;
        }

        while(isdigit(nextchar)) {               //reading and assigning until we reach a non number character
            if (index >= buffer_size - 1) {
                realloc_value(&current_token.value, &buffer_size);
            }
            current_token.value[index++] = (char) nextchar;
            nextchar = getc(stdin);
            Column_Number++;
        }
    }
    if(current_token.value[index-1] == 'e' ||   //If statement checks for incomplete 
        current_token.value[index-1] == 'E' ||  //exponential and decimal numbers.
        current_token.value[index-1] == '.' ||
        current_token.value[index-1] == '-' ||
        current_token.value[index-1] == '+' )
        {
        ERRORLEX(ERR_LEX, "Number incomplete on line %d, column %d.\n", Line_Number, Column_Number);
    }       
    
    ungetc(nextchar, stdin);   //returning a character back to the input

    current_token.value[index] = '\0';  //terminating the array of chars with null string
    
    //printf("%s\n", current_token.value);
    //printf("%d\n", current_token.type);
    
    return current_token;              //returning processed number token
}

/**
 * @brief            Function which processes tokens of type string.
 * 
 * @return           Functions returns processed token.
 */
Token process_String_Token() {
        
    Token current_token;
    char nextchar;
    int buffer_size = 2;
    int index = 0;

    current_token.type = tokentype_string;

    init_value(&current_token.value, buffer_size); //initializing value

    //while reads until we find quotation marks or we reach the end of line    
    while((nextchar = getc(stdin)) != '"' && nextchar != '\n') { 
        
        Column_Number++;

        if (index >= buffer_size - 1) {         //reallocating value if needed
            realloc_value(&current_token.value, &buffer_size);
        }
        if(nextchar == EOF) {     //if the string is terminated incorrectly and we reach EOF
            ERRORLEX(ERR_LEX, "String incomplete on line %d, column %d.\n", Line_Number, Column_Number);
        }

        //if statement for handling all escape sequences and hexadecimal numbers in string.
        if(nextchar == 92) {    //checking for a backslash
            
            nextchar = getc(stdin);
            Column_Number++;
            //correctly assigning each escape sequence directly into string value
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
            
            //logic for transforming hexadecimal numbers
            else if(nextchar == 'x') {
                char hex_str[3] = {0};  //initializing buffer string to all zeroes
                
                for(int i = 0; i < 2; i++) {    //Searching for hexadecimal number
                    if(((nextchar = getc(stdin)) >= '0' && nextchar <= '9') ||
                        (nextchar >= 'a' && nextchar <= 'f') || 
                        (nextchar >= 'A' && nextchar <= 'F'))    
                    {
                    hex_str[i] = nextchar; //putting the number into an array
                    Column_Number++;
                    }
                    else {
                        ERRORLEX(ERR_LEX, "Hexadecimal number incorrect on line %d, column %d.\n", Line_Number, Column_Number);
                    }
                }
                long dec_value = strtol(hex_str, NULL, 16); //transforming the number from hexadecimal to decimal
                
                int chars_needed = snprintf(NULL, 0, "%ld", dec_value) + 1; //calculating how many characters we are writing
                while (index + chars_needed >= buffer_size) {               //and raellocating if size exceeds allocated size
                    realloc_value(&current_token.value, &buffer_size);
                }
                index += snprintf(current_token.value + index, chars_needed, "%ld", dec_value); //updating index
            }
            else {
                ERRORLEX(ERR_LEX, "Escape sequence incorrect on line %d, column %d.\n", Line_Number, Column_Number);
            }
        }
        
        //If its none of the above, just write it to token value.
        else {
            current_token.value[index++] = nextchar;
        }
    }
    //If string was interrupted by the end of line, its incorrectly terminated
    if(nextchar == '\n') {
        ERRORLEX(ERR_LEX, "String incorrect on line %d, column %d.\n", Line_Number, Column_Number);
    }
    Column_Number++;

    current_token.value[index] = '\0';  //Terminating the string with a null string
    //printf("%s\n", current_token.value);
    //printf("%d\n", current_token.type);

    return current_token;   //Return processed token of type string.
}

/**
 * @brief Function which processes ID token.
 * 
 * @param firstchar  First character of the token.
 * 
 * @return The function returns processed ID token.
 */
Token process_ID_Token(char firstchar) {
    
    Token current_token;
    char nextchar;
    int buffer_size = 2;
    int index = 0;

    init_value(&current_token.value, buffer_size); //initializing value of token
    
    if(isalpha(firstchar) || firstchar == '_') {  //ID must start with a letter or an underscore
        current_token.type = tokentype_id;
    }

    else {
        ERRORLEX(ERR_LEX, "Invalid ID on line %d, column %d.\n", Line_Number, Column_Number);
    }
    
    current_token.value[index++] = firstchar;   //putting the first character of an ID into array.

    //reading until we find a character not allowed in ID
    while((isalpha(nextchar = getc(stdin))) || isdigit(nextchar) || nextchar == '_') {
        if (index >= buffer_size - 1) {
            realloc_value(&current_token.value, &buffer_size); //reallocating memory if needed
        }
        Column_Number++;
        current_token.value[index++] = nextchar;
    }
    current_token.value[index] = '\0'; //terminating character array with null string

    ungetc(nextchar, stdin);      //return the last read character back since we dont need it

    is_keyword(current_token.value, &current_token);    //decide whether the ID is a keyword or not

    //printf("%s\n", current_token.value);
    //printf("%d\n", current_token.type);

    return current_token; //return processed token of type ID
}

/**
 * @brief Function which processes the prologue token of program.
 * 
 * @return Processed token of type import.
 */
Token process_Import() {
    
    Token current_token;
    current_token.type = tokentype_import;
    current_token.value = NULL;

    const char *keyword = "import"; //char keyword for comparing
    char nextchar;
    int i = 0;

    while (keyword[i] != '\0') {
        nextchar = getc(stdin);
        Column_Number++;
        if (nextchar != keyword[i]) { //error if we find a mismatch
            ERRORLEX(ERR_LEX, "Import incorrect on line %d, column %d.\n", Line_Number, Column_Number);  
        }
        i++; 
    }
    
    //printf("%d\n", current_token.type);
    return current_token;
}

/**
 * @brief Function for processing multiline string token.
 * 
 * @return Proccessed multiline string token.
 */
Token process_Multiline_String_Token() {
    char nextchar;
    Token current_token;
    int index = 0;
    int buffer_size = 2;

    init_value(&current_token.value, buffer_size); //initalize value
    
    if((nextchar = getc(stdin)) == '\\') {   //multiline string begins with two backslashes
        current_token.type = tokentype_string;

        while(1) {
                
            if (index >= buffer_size - 1) {
                realloc_value(&current_token.value, &buffer_size);  //reallocate memory if needed
            }
            nextchar = getc(stdin);
            if(nextchar == EOF) {   //if you reached the end of file, break out of the loop
                break;
            }
            if(nextchar == '\n') {  //Checking if the multiline continues on the next line
                Line_Number++;
                char tempchar;
                while((tempchar = getc(stdin)) != EOF && isspace(tempchar)) { //skipping whitespaces
                    if(tempchar == '\n') { //We reached the end of line, break
                        break;
                    }
                }
                
                if(tempchar == '\\' && (nextchar = getc(stdin)) == '\\') { //check if multiline continues on next line
                    current_token.value[index++] = '\n';    //put EOL into the string if multiline continues
                    continue;
                }
                else {
                    Line_Number--;  //decrement line number if the string doesnt continue
                    ungetc(tempchar, stdin);   //return back the characters we read and dont need
                    ungetc(nextchar, stdin);
                    break;
                }
            }
            else {  //default case, assign char to array
                current_token.value[index++] = nextchar;
            }
        }
        current_token.value[index] = '\0'; //terminate string with a null string
    }
    else {
        ERRORLEX(ERR_LEX, "Invalid character on line %d, column %d.\n", Line_Number, Column_Number);
    }

    //printf("%s\n", current_token.value);
    //printf("%d\n", current_token.type);

    return current_token; //Return processed token
}

//TODO FUNCKCIA NA BUILTIN FUNKCIE

 // int main() {

     // for(int i = 0; i < 10; i++) {
     //     getToken();
    //  }
   // free_all_values();
    //reset_scanner();
 //   printf("\nEND\n");
 //   for(int i = 0; i < 10; i++) {
 //       getToken();
 //   }
   // free_all_values();

 //   return 0;
 // }

/* END OF FILE scanner.c */
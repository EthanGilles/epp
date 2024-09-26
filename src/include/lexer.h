#ifndef LEXER_H
#define LEXER_H 
#include "token.h"

/* Holds the state of the lexer */
typedef struct LEXER_STRUCT 
{
  char c;             /* Current character we are on */
  unsigned int index; /* Index in the file */
  char* contents;     /* Source code */
} lexer_T;


lexer_T* init_lexer(char* contents);

/* Move our pointer to the next character in the content */
void lexer_advance(lexer_T* lexer);

/* Skip over whitespace in the content we dont care about */
void lexer_skip_whitespace(lexer_T * lexer);

/* Get the next token in the contents 
 * Most of the logic is in this function */
token_T* lexer_get_next_token(lexer_T* lexer);

/* Defines how to parse a string */
token_T* lexer_collect_string(lexer_T* lexer);

token_T* lexer_collect_id(lexer_T* lexer);

token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token);

char* lexer_get_current_char_as_string(lexer_T* lexer);

#endif 



#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/lexer.h"


lexer_T* init_lexer(char* contents) 
{
  lexer_T* lexer = calloc(1, sizeof(struct LEXER_STRUCT));
  lexer->contents = contents;
  lexer->index = 0;
  lexer->c = contents[lexer->index];

  return lexer;
}

/* Bounds check to make sure the lexer doesn't go past the length 
 * of the contents. */
void lexer_advance(lexer_T* lexer)
{
  if(lexer->c != '\0' && lexer->index < strlen(lexer->contents))
  {
    lexer->index += 1;
    lexer->c = lexer->contents[lexer->index];
  }
}

void lexer_skip_whitespace(lexer_T * lexer)
{
  /* 10 is the code for \n */
  while(lexer->c == ' ' || lexer-> c == 10)
  {
    lexer_advance(lexer);
  }
}

/* returns the next token from the lexer */
token_T* lexer_get_next_token(lexer_T* lexer)
{
  while(lexer->c != '\0' && lexer->index < strlen(lexer->contents))
  {
    /* Skip over whitespace to the next thing written */
    if(lexer->c == ' ' || lexer-> c == 10)
      lexer_skip_whitespace(lexer);

    if(isalnum(lexer->c))
      return lexer_collect_id(lexer);

    /* Double quotes means a string is coming */
    if(lexer->c == '"')
      return lexer_collect_string(lexer);

    /* Return a token based on a single character */
    switch(lexer->c)
    {
      case '=':  return lexer_advance_with_token(lexer, init_token(TOKEN_EQUALS, lexer_get_current_char_as_string(lexer))); break;
      case ';':  return lexer_advance_with_token(lexer, init_token(TOKEN_SEMI, lexer_get_current_char_as_string(lexer))); break;
      case '(':  return lexer_advance_with_token(lexer, init_token(TOKEN_LPAREN, lexer_get_current_char_as_string(lexer))); break;
      case ')':  return lexer_advance_with_token(lexer, init_token(TOKEN_RPAREN, lexer_get_current_char_as_string(lexer))); break;
    }
  }

  /* No more tokens left */
  return (void*)0;
}

/* Get a string from inside double quotes 
 * Expecting: "Hello World!" 
 * So it will grab: Hello World! 
 * WITHOUT the quotes. */
token_T* lexer_collect_string(lexer_T* lexer)
{
  /* skip the opening quote */
  lexer_advance(lexer);

  /* Allocate enough for one character */
  char* value = calloc(1, sizeof(char));
  value[0] = '\0';

  while(lexer->c != '"')
  {
    char * s = lexer_get_current_char_as_string(lexer);

    /* Reallocate every time a new character is added */
    value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
    strcat(value, s);
    lexer_advance(lexer);
  }

  /* skip the closing quote */
  lexer_advance(lexer);

  return init_token(TOKEN_STRING, value);
}

/* An ID labels a variable. Either a type or a variable name */
token_T* lexer_collect_id(lexer_T* lexer)
{
  char* value = calloc(1, sizeof(char));
  value[0] = '\0';

  while(isalnum(lexer->c))
  {
    char * s = lexer_get_current_char_as_string(lexer);
    value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
    strcat(value, s);
    lexer_advance(lexer);
  }

  return init_token(TOKEN_ID, value);
}

token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token)
{
  lexer_advance(lexer);
  return token;
}

char* lexer_get_current_char_as_string(lexer_T* lexer)
{
  char* str = calloc(2, sizeof(char));
  str[0] = lexer->c;
  /* empty char slot */
  str[1] = '\0';

  return str;
}

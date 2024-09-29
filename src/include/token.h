#ifndef TOKEN_H
#define TOKEN_H 

typedef struct TOKEN_STRUCT 
{
  /*    TYPE            | # 
   * -------------------|---
   * ID                 | 0 
   * EQUALS             | 1 
   * STRING             | 2 
   * SEMICOLON          | 3 
   * LEFT PARENTHESIS   | 4 
   * RIGHT PARENTHESIS  | 5 
   * */
  enum 
  {
    TOKEN_ID,
    TOKEN_EQUALS,
    TOKEN_STRING,
    TOKEN_SEMI,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_COMMA,
    TOKEN_EOF, /* end of file */
  } type;

  /* The string that this token is holding */
  char* value;

} token_T;

/* Make a new token */
token_T* init_token(int type, char* value);

#endif 

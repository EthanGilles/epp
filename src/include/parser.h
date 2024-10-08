#ifndef PARSER_H
#define PARSER_H 
#include "lexer.h"
#include "AST.h"
#include "scope.h"

/* Keywords */
const static char* VAR_DEF_SYNTAX = "set";
const static char* FUNCTION_DEF_SYNTAX = "fn";

typedef struct PARSER_STRUCT 
{
  lexer_T* lexer;
  token_T* current_token;
  token_T* prev_token;
  scope_T* scope;
} parser_T;

parser_T* init_parser(lexer_T* lexer);

/* With each expected token we advance and go to the next token 
 * if we find an unexpecte token we can throw an error. */
void parser_eat(parser_T* parser, int token_type);

/* Will return the source code as an AST */
AST_T* parser_parse(parser_T* parser, scope_T* scope);

AST_T* parser_parse_statement(parser_T* parser, scope_T* scope);

AST_T* parser_parse_statements(parser_T* parser, scope_T* scope);

/* Parse expression will parse a mathematical expression */
AST_T* parser_parse_expr(parser_T* parser, scope_T* scope);

/* Will parse things that multiply together */
AST_T* parser_parse_factor(parser_T* parser, scope_T* scope);

/* WIll parse things that add together */
AST_T* parser_parse_term(parser_T* parser, scope_T* scope);

/* Will return AST node with type function call */
AST_T* parser_parse_function_call(parser_T* parser, scope_T* scope);

/* Will return AST node with type function call */
AST_T* parser_parse_function_definition(parser_T* parser, scope_T* scope);

/* Will return AST node with type variable definition */
AST_T* parser_parse_variable_definition(parser_T* parser, scope_T* scope);

/* Will return AST node with type variable */
AST_T* parser_parse_variable(parser_T* parser, scope_T* scope);

/* Will return AST node with type string */
AST_T* parser_parse_string(parser_T* parser, scope_T* scope);

/* Will return AST node with type ID */
AST_T* parser_parse_id(parser_T* parser, scope_T* scope);


#endif 

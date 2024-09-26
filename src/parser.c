#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/lexer.h"
#include "include/AST.h"
#include "include/parser.h"

parser_T* init_parser(lexer_T* lexer)
{
  parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
  parser->lexer = lexer;
  parser->current_token = lexer_get_next_token(lexer);

  return parser;
}

/* Destroy a token. If its not the token we think it is then
 * throw an error */
void parser_eat(parser_T* parser, int token_type)
{
  if(parser->current_token->type == token_type)
    parser->current_token = lexer_get_next_token(parser->lexer);
  else
  {
    printf("Unexpected token '%s', with type %d",
           parser->current_token->value,
           parser->current_token->type);
    exit(1);
  }
}

/* Main entry point for parsing */
AST_T* parser_parse(parser_T* parser)
{
  return parser_parse_statements(parser);
}

AST_T* parser_parse_statement(parser_T* parser)
{
  switch(parser->current_token->type)
  {
    case TOKEN_ID: return parser_parse_id(parser);
  }
}

AST_T* parser_parse_statements(parser_T* parser)
{
  /* Creating a compound type AST node and initializing its 
   * value to be the size of one AST_STRUCT pointer */
  AST_T* compound = init_ast(AST_COMPOUND);
  compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*));

  /* Expecting a statement, so we parse it and add it to the list */
  AST_T* ast_statement = parser_parse_statement(parser);
  compound->compound_value[0] = ast_statement;

  /* While we still have semicolons, we will expect more statements 
   * So we will parse the rest of the statements and add them to the list */
  while(parser->current_token->type == TOKEN_SEMI)
  {
    /* take out the semi colon */
    parser_eat(parser, TOKEN_SEMI);

    /* Parse statement 
     * Increase List size 
     * Reallocate space for another item 
     * Add the item to the last spot in the list */
    AST_T* ast_statement = parser_parse_statement(parser);
    compound->compound_size +=1;
    compound->compound_value = realloc(compound->compound_value, 
                                       compound->compound_size * sizeof(struct AST_STRUCT*));

    compound->compound_value[compound->compound_size-1] = ast_statement;

  }

  return compound;
}

AST_T* parser_parse_expr(parser_T* parser)
{
  switch(parser->current_token->type)
  {
    case TOKEN_STRING: return parser_parse_string(parser);
  }
}

AST_T* parser_parse_factor(parser_T* parser)
{

}

AST_T* parser_parse_term(parser_T* parser)
{

}

AST_T* parser_parse_function_call(parser_T* parser)
{

}


AST_T* parser_parse_variable_definition(parser_T* parser)
{
  parser_eat(parser, TOKEN_ID); /* DESTROY -> var */
  char* variable_name = parser->current_token->value; /* STORE -> name */
  parser_eat(parser, TOKEN_ID); /* DESTROY -> name */
  parser_eat(parser, TOKEN_EQUALS); /* DESTROY -> = */
  AST_T* variable_value = parser_parse_expr(parser); /* STORE -> "Hello World!" */

  /* AST node to return */
  AST_T* variable_def = init_ast(AST_VARIABLE_DEFINITION);
  variable_def->variable_definition_variable_name = variable_name;
  variable_def->variable_definition_value = variable_value;

  return variable_def;
}

AST_T* parser_parse_variable(parser_T* parser)
{
  char* token_value = parser->current_token->value;
  parser_eat(parser, TOKEN_ID); /* var name OR print(name) */

  /* If we find a parenthesis this is actually a function call */
  if(parser->current_token->type == TOKEN_LPAREN)
    return parser_parse_function_call(parser);

  /* Assume we are getting a variable, return the name */
  AST_T* ast_variable = init_ast(AST_VARIABLE);
  ast_variable->variable_name = token_value;

  return ast_variable;
}

AST_T* parser_parse_string(parser_T* parser)
{
  AST_T* ast_string = init_ast(AST_STRING);

  ast_string->string_value = parser->current_token->value;
  
  parser_eat(parser, TOKEN_STRING);

  return ast_string;
}

AST_T* parser_parse_id(parser_T* parser)
{
  if(strcmp(parser->current_token->value, "var") == 0)
  {
    return parser_parse_variable_definition(parser);
  }
  else 
  {
    return parser_parse_variable(parser);
  }

}


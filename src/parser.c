#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/lexer.h"
#include "include/AST.h"
#include "include/parser.h"
#include "include/scope.h"


parser_T* init_parser(lexer_T* lexer)
{
  parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
  parser->lexer = lexer;
  parser->current_token = lexer_get_next_token(lexer);
  parser->prev_token = parser->current_token;

  parser->scope = init_scope();

  return parser;
}

/* Destroy a token. If its not the token we think it is then
 * throw an error */
void parser_eat(parser_T* parser, int token_type)
{
  if(parser->current_token->type == token_type)
  {
    parser->prev_token = parser->current_token;
    parser->current_token = lexer_get_next_token(parser->lexer);
  }
  else
  {
    printf("Unexpected token '%s', with type %d\n",
           parser->current_token->value,
           parser->current_token->type);
    exit(1);
  }
}

/* Main entry point for parsing */
AST_T* parser_parse(parser_T* parser, scope_T* scope)
{
  return parser_parse_statements(parser, scope);
}

AST_T* parser_parse_statement(parser_T* parser, scope_T* scope)
{
  switch(parser->current_token->type)
  {
    case TOKEN_ID: return parser_parse_id(parser, scope);
  }
  
  return init_ast(AST_NOOP);
}

AST_T* parser_parse_statements(parser_T* parser, scope_T* scope)
{
  /* Creating a compound type AST node and initializing its 
   * value to be the size of one AST_STRUCT pointer */
  AST_T* compound = init_ast(AST_COMPOUND);
  compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*));
  compound->scope = scope;

  /* Expecting a statement, so we parse it and add it to the list */
  AST_T* ast_statement = parser_parse_statement(parser, scope);
  ast_statement->scope = scope;
  compound->compound_value[0] = ast_statement;
  compound->compound_size += 1;

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
    AST_T* ast_statement = parser_parse_statement(parser, scope);

    if(ast_statement) 
    {
    compound->compound_size +=1;
    compound->compound_value = 
      realloc(compound->compound_value, 
              compound->compound_size * sizeof(struct AST_STRUCT*));

    compound->compound_value[compound->compound_size-1] = ast_statement;
    }
  }

  return compound;
}

AST_T* parser_parse_expr(parser_T* parser, scope_T* scope)
{
  switch(parser->current_token->type)
  {
    case TOKEN_STRING: return parser_parse_string(parser, scope);
    case TOKEN_ID: return parser_parse_id(parser, scope);
  }
  
  return init_ast(AST_NOOP);
}

AST_T* parser_parse_factor(parser_T* parser, scope_T* scope)
{

}

AST_T* parser_parse_term(parser_T* parser, scope_T* scope)
{
}


AST_T* parser_parse_function_call(parser_T* parser, scope_T* scope)
{
  AST_T* function_call = init_ast(AST_FUNCTION_CALL);
  /* We are at a left parenthesis so we need to go 
   * back one to get the function name */
  function_call->function_call_name = parser->prev_token->value;
  parser_eat(parser, TOKEN_LPAREN);

  function_call->function_call_arguments = calloc(1, sizeof(struct AST_STRUCT*));

  /* Expecting an argument to the function, so we parse it and add it to the list */
  AST_T* ast_expr = parser_parse_expr(parser, scope);
  function_call->function_call_arguments[0] = ast_expr;
  function_call->function_call_arguments_size += 1;

  /* While we still have commas, we will expect more arguments 
   * So we will parse the rest of the statements and add them to the list */
  while(parser->current_token->type == TOKEN_COMMA)
  {
    /* take out the comma */
    parser_eat(parser, TOKEN_COMMA);

    AST_T* ast_expr = parser_parse_expr(parser, scope);
    function_call-> function_call_arguments_size += 1;
    function_call-> function_call_arguments = 
      realloc(function_call->function_call_arguments, 
              function_call->function_call_arguments_size * 
              sizeof(struct AST_STRUCT*));

    function_call->function_call_arguments[function_call->function_call_arguments_size-1] = ast_expr;
  }

  parser_eat(parser, TOKEN_RPAREN);
  function_call->scope = scope;
  return function_call;
}

AST_T* parser_parse_function_definition(parser_T* parser, scope_T* scope)
{
  AST_T* ast = init_ast(AST_FUNCTION_DEFINITION);
  parser_eat(parser, TOKEN_ID); /* DESTROY -> function */

  char* function_name = parser->current_token->value; /* STORE -> fn name in our fn definition */
  ast->function_definition_name = calloc(strlen(function_name)+1, sizeof(char));
  strcpy(ast->function_definition_name, function_name);
  parser_eat(parser, TOKEN_ID); /* DESTROY -> name */

  parser_eat(parser, TOKEN_LPAREN); /* DESTROY -> ( */

  /* parse the arguments of the function */
  ast->function_definition_args = calloc(1, sizeof(struct AST_STRUCT*));
  AST_T* arg = parser_parse_variable(parser, scope);
  ast->function_definition_args_size += 1;
  ast->function_definition_args[ast->function_definition_args_size-1] = arg;


  while(parser->current_token->type == TOKEN_COMMA) {
    parser_eat(parser, TOKEN_COMMA); /* DESTROY -> , */
    ast->function_definition_args_size += 1;

    /* Add another slot to the list */
    ast->function_definition_args = realloc(
      ast->function_definition_args, 
      ast->function_definition_args_size * sizeof(struct AST_STRUCT*));

    AST_T* arg = parser_parse_variable(parser, scope);
    ast->function_definition_args[ast->function_definition_args_size-1] = arg;
  }

  parser_eat(parser, TOKEN_RPAREN); /* DESTROY -> ) */

  parser_eat(parser, TOKEN_LBRACE); /* DESTROY -> { */
  /* parse the body of the function */
  ast->function_definition_body = parser_parse_statements(parser, scope);

  parser_eat(parser, TOKEN_RBRACE); /* DESTROY -> } */

  /* assign it a scope and return */
  ast->scope = scope;
  return ast;
}

AST_T* parser_parse_variable_definition(parser_T* parser, scope_T* scope)
{
  parser_eat(parser, TOKEN_ID); /* DESTROY -> var */
  char* variable_name = parser->current_token->value; /* STORE -> name */
  parser_eat(parser, TOKEN_ID); /* DESTROY -> name */
  parser_eat(parser, TOKEN_EQUALS); /* DESTROY -> = */
  AST_T* variable_value = parser_parse_expr(parser, scope); /* STORE -> "Hello!" */

  /* AST node to return */
  AST_T* variable_def = init_ast(AST_VARIABLE_DEFINITION);
  variable_def->variable_definition_variable_name = variable_name;
  variable_def->variable_definition_value = variable_value;
  variable_def->scope = scope;

  return variable_def;
}

AST_T* parser_parse_variable(parser_T* parser, scope_T* scope)
{
  char* token_value = parser->current_token->value;
  parser_eat(parser, TOKEN_ID); /* var name OR print(name) */

  /* If we find a parenthesis this is actually a function call */
  if(parser->current_token->type == TOKEN_LPAREN)
    return parser_parse_function_call(parser, scope);

  /* Assume we are getting a variable, return the name */
  AST_T* ast_variable = init_ast(AST_VARIABLE);
  ast_variable->variable_name = token_value;

  /* assign it a scope and return */
  ast_variable->scope = scope;
  return ast_variable;
}

AST_T* parser_parse_string(parser_T* parser, scope_T* scope)
{
  AST_T* ast_string = init_ast(AST_STRING);

  ast_string->string_value = parser->current_token->value;
  
  parser_eat(parser, TOKEN_STRING);

  /* assign it a scope and return */
  ast_string->scope = scope;
  return ast_string;
}

AST_T* parser_parse_id(parser_T* parser, scope_T* scope)
{
  if(strcmp(parser->current_token->value, VAR_DEF_SYNTAX) == 0)
  {
    return parser_parse_variable_definition(parser, scope);
  }
  else if(strcmp(parser->current_token->value, FUNCTION_DEF_SYNTAX) == 0)
  {
    return parser_parse_function_definition(parser, scope);
  }
  else 
  {
    return parser_parse_variable(parser, scope);
  }
}


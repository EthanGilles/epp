#include "include/visitor.h"
#include "include/AST.h"
#include "include/scope.h"
#include <stdio.h>
#include <string.h>

static AST_T* 
builtin_function_print(visitor_T* visitor, AST_T** args, int args_size)
{
  for(int i = 0; i < args_size; i++)
  {
    AST_T* visited_ast = visitor_visit(visitor, args[i]);

    switch(visited_ast->type)
    {
      case AST_STRING: printf("%s\n", visited_ast->string_value); break;
      default: printf("Unexpected type %d\n", visited_ast->type); break;
    }

  }

  return init_ast(AST_NOOP);
}

visitor_T* init_visitor()
{
  visitor_T* visitor = calloc(1, sizeof(struct VISITOR_STRUCT));

  return visitor;
}

AST_T* visitor_visit(visitor_T* visitor, AST_T* node)
{
  switch(node->type)
  {
    case AST_VARIABLE_DEFINITION: 
      return visitor_visit_variable_definition(visitor, node); break;
    case AST_FUNCTION_DEFINITION: 
      return visitor_visit_function_definition(visitor, node); break;
    case AST_VARIABLE: 
      return visitor_visit_variable(visitor, node); break;
    case AST_FUNCTION_CALL: 
      return visitor_visit_function_call(visitor, node); break;
    case AST_STRING: 
      return visitor_visit_string(visitor, node); break;
    case AST_COMPOUND: 
      return visitor_visit_compound(visitor, node); break;
    case AST_NOOP: 
      return node; break;
  }

  printf("Unknown AST type - %d\n", node->type);
  exit(1);

  return init_ast(AST_NOOP);
}

/* Store the variable in memory */
AST_T* visitor_visit_variable_definition(visitor_T* visitor, AST_T* node)
{
  scope_add_variable_defintion(node->scope, node);

  return node;
}

AST_T* visitor_visit_variable(visitor_T* visitor, AST_T* node)
{
  AST_T* vdef = scope_get_variable_definition(node->scope, node->variable_name);

  if(vdef !=(void*)0)
  {
    return visitor_visit(visitor, vdef->variable_definition_value);
  }

  printf("Undefined variable `%s`\n", node->variable_name);
  return node;
}

AST_T* visitor_visit_function_definition(visitor_T* visitor, AST_T* node)
{
  scope_add_function_defintion(node->scope, node);

  return node;
}

AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node)
{
  if(strcmp(node->function_call_name, "print") == 0)
  {
    return builtin_function_print(visitor, 
    node->function_call_arguments, node->function_call_arguments_size);
  }

  AST_T* fn_def = scope_get_function_definition(
    node->scope, 
    node->function_call_name
  );

  if(fn_def == (void*)0)
  {
    printf("Undefined function `%s`\n", node->function_call_name);
    exit(1);
  }

  for(int i = 0; i < (int) node->function_call_arguments_size; i++)
  {
    /* Grab variable from the function definition arguments */
    AST_T* ast_var = (AST_T*) fn_def->function_definition_args[i];
    AST_T* ast_value = (AST_T*) node->function_call_arguments[i];

    /* Create variable definition */
    AST_T* variable_def = init_ast(AST_VARIABLE_DEFINITION);

    /* Copy value of the variable over */
    variable_def->variable_definition_value = ast_value;

    /* Copy the name of the variable over */
    variable_def->variable_definition_variable_name = (char*)calloc(
        strlen(ast_var->variable_name) + 1, 
        sizeof(char)
      );
    
    strcpy(
      variable_def->variable_definition_variable_name, 
      ast_var->variable_name
    );

    /* Push the variable definition into the scope of the function body */
    scope_add_variable_defintion(
      fn_def->function_definition_body->scope,
      variable_def
    );
  }

  return visitor_visit(visitor, fn_def->function_definition_body);
}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* node)
{
  return node;
}

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* node)
{
  for(int i = 0; i < node->compound_size; i++)
  {
    visitor_visit(visitor, node->compound_value[i]);
  }

  return init_ast(AST_NOOP);
}

#include <string.h>
#include "include/scope.h"
#include "include/AST.h"

scope_T* init_scope()
{
  scope_T* scope = calloc(1, sizeof(struct SCOPE_STRUCT));

  scope->function_defintions = (void*)0;
  scope->function_defintions_size = 0;

  scope->variable_definitions = (void*)0;
  scope->variable_definitions_size = 0;

  return scope;
}

AST_T* scope_add_function_defintion(scope_T* scope, AST_T* fn_def)
{
  scope->function_defintions_size += 1;

  /* if we have no definitions yet, allocate memory for 1 
   * if we do have defintitions, allocate memory for one more */
  if(scope->function_defintions == (void*)0) 
  {
    scope->function_defintions = calloc(1, sizeof(struct AST_STRUCT*));
  }
  else
  {
    scope->function_defintions = realloc(scope->function_defintions, 
      sizeof(struct AST_STRUCT*) * scope->function_defintions_size);
  }

  scope->function_defintions[scope->function_defintions_size-1] = fn_def;
  return fn_def;
}


AST_T* scope_get_function_definition(scope_T* scope, char* fn_name)
{
  for(int i = 0; i < scope->function_defintions_size; i++)
  {
    AST_T* fn_def = scope->function_defintions[i];

    if(strcmp(fn_def->function_definition_name, fn_name) == 0)
    {
      return fn_def;
    }
  }

  return (void*)0;
}


AST_T* scope_add_variable_defintion(scope_T* scope, AST_T* def)
{

  if(scope->variable_definitions == (void*)0)
  {
    scope->variable_definitions = calloc(1, sizeof(struct AST_STRUCT*));
    scope->variable_definitions[0] = def;
    scope->variable_definitions_size += 1;
  }
  else
  {
    scope->variable_definitions_size += 1;
    scope->variable_definitions = realloc(
      scope->variable_definitions, 
      sizeof(struct AST_STRUCT*) * scope->variable_definitions_size);
    scope->variable_definitions[scope->variable_definitions_size-1] = def;
  }

  return def;
}

AST_T* scope_get_variable_definition(scope_T* scope, char* name)
{
  for(int i = 0; i < scope->variable_definitions_size ; i++)
  {
    AST_T* def = scope->variable_definitions[i];
    if(strcmp(def->variable_definition_variable_name, name) == 0)
    {
      return def;
    }
  }

  return (void*)0;
}

#ifndef SCOPE_H
#define SCOPE_H 
#include "AST.h"

typedef struct SCOPE_STRUCT 
{
  AST_T** function_defintions;
  size_t function_defintions_size;

  AST_T** variable_definitions;
  size_t variable_definitions_size;

} scope_T;

scope_T* init_scope();

AST_T* scope_add_function_defintion(scope_T* scope, AST_T* fn_def);

AST_T* scope_get_function_definition(scope_T* scope, char* fn_name);

AST_T* scope_add_variable_defintion(scope_T* scope, AST_T* def);

AST_T* scope_get_variable_definition(scope_T* scope, char* name);

#endif 

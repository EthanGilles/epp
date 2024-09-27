#include <stdio.h>
#include "include/lexer.h"
#include "include/token.h"
#include "include/parser.h"

int main(int argc, char *argv[])
{
  lexer_T* lexer = init_lexer(
    "var name = \"Ethan\";\n"
    "print(name);\n"
  );

  parser_T* parser = init_parser(lexer);
  AST_T* root = parser_parse(parser);

  return 0;
}

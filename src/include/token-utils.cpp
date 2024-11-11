#pragma once
#include "tokenization.hpp"

std::string to_string(const TokenType type)
{
  /* Returns a literal string representation of the token */
  switch(type) 
  {
    case TokenType::GOODBYE:
      return "`goodbye`";
    case TokenType::PRINT:
      return "`print`";
    case TokenType::INT_LIT:
      return "int_lit";
    case TokenType::ID:
      return "id";
    case TokenType::SET:
      return "`set`";
    case TokenType::RESET:
      return "`reset`";
    case TokenType::IF:
      return "`if`";
    case TokenType::ELSIF:
      return "`elsif`";
    case TokenType::ELSE:
      return "`else`";
    case TokenType::PLEASE:
      return "`please`";
    case TokenType::PLEASE_C:
      return "`PLEASE`";
    case TokenType::PLUS:
      return "`+`";
    case TokenType::STAR:
      return "`*`";
    case TokenType::MINUS:
      return "`-`";
    case TokenType::FSLASH:
      return "`/`";
    case TokenType::PERCENT:
      return "`%`";
    case TokenType::COMMA:
      return "`,`";
    case TokenType::LCURLY:
      return "`{`";
    case TokenType::RCURLY:
      return "`}`";
    case TokenType::LBRACKET:
      return "`[`";
    case TokenType::RBRACKET:
      return "`]`";
    case TokenType::SEMI:
      return "`;`";
    case TokenType::EQUALS:
      return "`=`";
    case TokenType::LPAREN:
      return "`(`";
    case TokenType::RPAREN:
      return "`)`";
    case TokenType::GT:
      return "`>`";
    case TokenType::LT:
      return "`<`";
    case TokenType::GTEQ:
      return "`>=`";
    case TokenType::LTEQ:
      return "`<=`";
    case TokenType::NOTEQ:
      return "`!=`";
    case TokenType::DBEQ:
      return "`==`";
    default:
      return "Token to_string not implemented";
  }
}

/* Returns true if token is a binary comparison */
bool is_bin_cmp(TokenType type)
{
  switch(type) {
    case TokenType::GT:
    case TokenType::LT:
    case TokenType::GTEQ:
    case TokenType::LTEQ:
    case TokenType::NOTEQ:
    case TokenType::DBEQ:
      return true;
    default:
      return false;
  }
}

/* Returns true if token is a binary operator */
bool is_bin_op(TokenType type)
{
  switch(type) {
    case TokenType::PLUS:
    case TokenType::STAR:
    case TokenType::MINUS:
    case TokenType::FSLASH:
    case TokenType::PERCENT:
    case TokenType::GT:
    case TokenType::LT:
    case TokenType::GTEQ:
    case TokenType::LTEQ:
    case TokenType::NOTEQ:
    case TokenType::DBEQ:
      return true;
    default:
      return false;
  }
}

/* Returns precedence of a binary operator 
 * multiplication = 2
 * division       = 2
 * modulo         = 2
 * addition       = 1
 * subtraction    = 1
 * comparison     = 1      */
std::optional<int> bin_prec(TokenType type)
{
  switch(type) {
    case TokenType::STAR:
    case TokenType::FSLASH:
    case TokenType::PERCENT:
      return 2;
    case TokenType::PLUS:
    case TokenType::MINUS:
      return 1;
    case TokenType::GT:
    case TokenType::LT:
    case TokenType::GTEQ:
    case TokenType::LTEQ:
    case TokenType::NOTEQ:
    case TokenType::DBEQ:
      return 0;
    default:
      return {};
  }
}


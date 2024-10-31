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
    case TokenType::SEMI:
      return "`;`";
    case TokenType::EQUALS:
      return "`=`";
    case TokenType::LPAREN:
      return "`(`";
    case TokenType::RPAREN:
      return "`)`";
    default:
      return "unknown";
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
      return true;
    default:
      return false;
  }
}

/* Returns precedence of a binary operator 
 * multiplication = 1
 * division       = 1
 * modulo         = 1
 * addition       = 0 
 * subtraction    = 0               */
std::optional<int> bin_prec(TokenType type)
{
  switch(type) {
    case TokenType::STAR:
    case TokenType::FSLASH:
    case TokenType::PERCENT:
      return 1;
    case TokenType::PLUS:
    case TokenType::MINUS:
      return 0;
    default:
      return {};
  }
}


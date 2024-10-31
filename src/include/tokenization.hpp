#pragma once
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <iostream>

enum class TokenType { 
  EXIT, // 'exit'
  SET, // 'set'
  RESET, // 'reset'
  IF, // 'if'
  ELSIF, // 'elsif'
  ELSE, // 'else'
  PLEASE, // 'please'
  PLEASE_C, // 'PLEASE'
  ID, // 'x'
  INT_LIT, // '59'
  SEMI, // ';'
  LPAREN, // '('
  RPAREN, // ')'
  LCURLY, // '{'
  RCURLY, //'}'
  EQUALS, // '='
  PLUS, // '+'
  STAR, // '*'
  MINUS, // '-'
  FSLASH, // '/'
  PERCENT, // '%'
};

std::string to_string(const TokenType type)
{
  switch(type) 
  {
    case TokenType::EXIT:
      return "`exit`";
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

struct Token {
  TokenType type;
  size_t line;
  std::optional<std::string> value;
};

class Tokenizer {
public:
  Tokenizer(const std::string& src) 
    : m_src(std::move(src)) {}


  std::vector<Token> tokenize() 
  {
    std::vector<Token> tokens;
    std::string buffer;
    size_t line_count = 1;

    while (peek().has_value())
    {
      /* Tokenize word */
      if (std::isalpha(peek().value()))
      {
        /* Add entire word to the buffer */
        buffer.push_back(consume());
        while (peek().has_value() && std::isalnum(peek().value()))
          buffer.push_back(consume());

        auto keyword = keywordMap.find(buffer); // Look for keyword
        if (keyword != keywordMap.end()) // Is a keyword
            tokens.push_back( {keyword->second, line_count} ); 
        else // Is identifier
            tokens.push_back( {TokenType::ID, line_count, buffer} );

        buffer.clear();
      }

      /* Tokenize number */
      else if (std::isdigit(peek().value()))
      {
        buffer.push_back(consume());
        while (peek().has_value() && std::isdigit(peek().value()))
          buffer.push_back(consume());

        tokens.push_back( {TokenType::INT_LIT, line_count, buffer} );
        buffer.clear();
      }

      /* One line comment */
      else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '/') {
        /* // comment example */
        consume(); // First slash
        consume(); // Second slash
        while (peek().has_value() && peek().value() != '\n') 
          consume(); // Skip everything until new line character
      }

      /* Multi-line comment */
      else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '*') {
        // /* comment example */
        consume();
        consume(); 
        while (peek().has_value()) {
          if (peek().value() == '*' && peek(1).has_value() && peek(1).value() == '/')
            break;
          consume(); // Skip everything until new line character
        }

        if (peek().has_value())
          consume();
        if (peek().has_value())
          consume();
      }

      else if (peek().value() == '\n')
      {
        consume();
        line_count++;
      }

      /* Look for single char tokens */
      else if (tokenMap.find(peek().value()) != tokenMap.end()) 
      {
        tokens.push_back( { tokenMap[peek().value()], line_count } );
        consume();
      }

      else if (std::isspace(peek().value()))
      {
        consume();
      }

      else
      {
          std::cerr << "Invalid token." << std::endl;
          exit(EXIT_FAILURE);
      }
    }
    m_index = 0;
    return tokens;
  }

private:
  [[nodiscard]] std::optional<char> peek(int offset = 0) const 
  {
    if (m_index + offset >= m_src.length())
      return {};
    else
     return m_src.at(m_index + offset);
  }

  char consume()
  {
    return m_src.at(m_index++);
  }


  std::unordered_map<char, TokenType> tokenMap = {
    {'(', TokenType::LPAREN},
    {')', TokenType::RPAREN},
    {'{', TokenType::LCURLY},
    {'}', TokenType::RCURLY},
    {';', TokenType::SEMI},
    {'=', TokenType::EQUALS},
    {'+', TokenType::PLUS},
    {'*', TokenType::STAR},
    {'-', TokenType::MINUS},
    {'/', TokenType::FSLASH},
    {'%', TokenType::PERCENT}
  };

  std::unordered_map<std::string, TokenType> keywordMap = {
      {"exit", TokenType::EXIT},
      {"set", TokenType::SET},
      {"reset", TokenType::RESET},
      {"if", TokenType::IF},
      {"elsif", TokenType::ELSIF},
      {"else", TokenType::ELSE},
      {"please", TokenType::PLEASE},
      {"PLEASE", TokenType::PLEASE_C},
  };

  const std::string m_src;
  size_t m_index = 0;
};

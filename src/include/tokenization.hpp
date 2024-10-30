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
  IF, // 'if'
  ELSIF, // 'elsif'
  ELSE, // 'else'
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
            tokens.push_back( {.type = keyword->second} ); 
        else // Is identifier
            tokens.push_back( {.type = TokenType::ID, .value = buffer} );

        buffer.clear();
      }

      /* Tokenize number */
      else if (std::isdigit(peek().value()))
      {
        buffer.push_back(consume());
        while (peek().has_value() && std::isdigit(peek().value()))
          buffer.push_back(consume());

        tokens.push_back({.type = TokenType::INT_LIT, .value = buffer});
        buffer.clear();
      }

      /* One line comment */
      else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '/') {
        /* // comment example */
        consume(); // First slash
        consume(); // Second slash
        while(peek().has_value() && peek().value() != '\n') 
          consume(); // Skip everything until new line character
      }

      /* Multi-line comment */
      else if (peek().value() == '/' && peek(1).has_value() && peek(1).value() == '*') {
        // /* comment example */
        consume(); // Slash
        consume(); // Star
        while(peek().has_value()) {
          if(peek().value() == '*' && peek(1).has_value() && peek(1).value() == '/')
            break;
          consume(); // Skip everything until new line character
        }

        // If comment is closed 
        if(peek().has_value())
          consume(); // Slash
        if(peek().has_value())
          consume(); // Star
      }


      /* Look for single char tokens */
      else if (tokenMap.find(peek().value()) != tokenMap.end()) 
      {
        tokens.push_back( {.type = tokenMap[peek().value()]} );
        consume();
      }

      else if(std::isspace(peek().value()))
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
      {"if", TokenType::IF},
      {"elsif", TokenType::ELSIF},
      {"else", TokenType::ELSE},
  };

  const std::string m_src;
  size_t m_index = 0;
};

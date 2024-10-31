#pragma once
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <iostream>

enum class TokenType { 
  GOODBYE, // 'goodbye'
  PRINT, // 'print'
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
  COMMA, // ','
};

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
    {'%', TokenType::PERCENT},
    {',', TokenType::COMMA}
  };

  std::unordered_map<std::string, TokenType> keywordMap = {
      {"goodbye", TokenType::GOODBYE},
      {"print", TokenType::PRINT},
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

#pragma once
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <iostream>
#include <cassert>

enum class TokenType { 
  GOODBYE, // 'goodbye'
  PRINT, // 'print'
  PRINTNL, // 'print'
  SET, // 'set'
  RESET, // 'reset'
  WHILE, // 'while'
  IF, // 'if'
  ELSIF, // 'elsif'
  ELSE, // 'else'
  PLEASE, // 'please'
  PLEASE_C, // 'PLEASE'
  ID, // 'x'
  INT_LIT, // '59'
  CHAR, // ''A''
  SEMI, // ';'
  QUOTE, // '''
  LPAREN, // '('
  RPAREN, // ')'
  LCURLY, // '{'
  RCURLY, //'}'
  EQUALS, // '='
  TO, // 'to'
  PLUS, // '+'
  STAR, // '*'
  MINUS, // '-'
  FSLASH, // '/'
  PERCENT, // '%'
  COMMA, // ','
  LT, // '<'
  LTEQ, // '<='
  GT, // '>'
  GTEQ, // '>='
  NOTEQ, // '!='
  NOT, // '!'
  DBEQ, // '=='
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

        auto keyword = keywordMap.find(buffer);
        if (keyword != keywordMap.end()) 
        {
          if(buffer == "space") 
            tokens.push_back( {TokenType::INT_LIT, line_count, "32"} );
          else if (buffer == "newline")
              tokens.push_back( {TokenType::INT_LIT, line_count, "10"} );
          else if (buffer == "true")
              tokens.push_back( {TokenType::INT_LIT, line_count, "1"} );
          else if (buffer == "false")
              tokens.push_back( {TokenType::INT_LIT, line_count, "0"} );
          else
            tokens.push_back( {keyword->second, line_count} ); 
        }
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

      /* double char tokens */
      else if (peek().value() == '<')
      {
        consume();
        if (peek().has_value() && peek().value() == '=')
        {
          tokens.push_back( {TokenType::LTEQ, line_count} );
          consume();
        }
        else
          tokens.push_back( {TokenType::LT, line_count} );
      }
      else if (peek().value() == '>')
      {
        consume();
        if (peek().has_value() && peek().value() == '=')
        {
          tokens.push_back( {TokenType::GTEQ, line_count} );
          consume();
        }
        else
          tokens.push_back( {TokenType::GT, line_count} );
      }

      else if (peek().value() == '!')
      {
        consume();
        if (peek().has_value() && peek().value() == '=')
        {
          tokens.push_back( {TokenType::NOTEQ, line_count} );
          consume();
        }
        else 
        {
          tokens.push_back( {TokenType::NOT, line_count} );
          assert(false && "`Not` not implemented");
        }
      }
      else if (peek().value() == '=')
      {
        consume();
        if(peek().has_value() && peek().value() == '=')
        {
          tokens.push_back( {TokenType::DBEQ, line_count} );
          consume();
        }
        else 
          tokens.push_back( {TokenType::EQUALS, line_count} );
      }
      else if (peek().value() == '\'')
      {
        consume();
        buffer.push_back(consume());
        if(peek().has_value() && peek().value() == '\'')
          consume();
        else
        {
          std::cerr << "[Tokenizer Error] expected closing quote for character" << std::endl;;
          exit(EXIT_FAILURE);
        }
        tokens.push_back( {TokenType::CHAR, line_count, buffer} );
        buffer.clear();
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
          std::cerr << "[Tokenizer Error] Invalid token." << std::endl;
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
    {',', TokenType::COMMA},
    {'\'', TokenType::QUOTE}
  };

  std::unordered_map<std::string, TokenType> keywordMap = {
      {"goodbye", TokenType::GOODBYE},
      {"while", TokenType::WHILE},
      {"print", TokenType::PRINT},
      {"printn", TokenType::PRINTNL},
      {"set", TokenType::SET},
      {"reset", TokenType::RESET},
      {"to", TokenType::EQUALS},
      {"if", TokenType::IF},
      {"elsif", TokenType::ELSIF},
      {"else", TokenType::ELSE},
      {"please", TokenType::PLEASE},
      {"PLEASE", TokenType::PLEASE_C},
      {"space", TokenType::INT_LIT},
      {"newline", TokenType::INT_LIT},
      {"true", TokenType::INT_LIT},
      {"false", TokenType::INT_LIT},
  };

  const std::string m_src;
  size_t m_index = 0;
};

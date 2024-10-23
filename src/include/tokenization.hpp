#pragma once
#include <cctype>
#include <string>
#include <vector>
#include <optional>
#include <iostream>

enum class TokenType { 
  EXIT, 
  INT_LIT, 
  SEMI,
  LPAREN,
  RPAREN,
  ID,
  SET,
  EQUALS, // '='
  PLUS, // '+'
  MULT, // '*'
  SUB, // '-'
  DIV, // '/'
};

bool is_bin_op(TokenType type)
{
  switch(type) {
    case TokenType::PLUS:
    case TokenType::MULT:
    case TokenType::SUB:
    case TokenType::DIV:
      return true;
    default:
      return false;
  }
}

std::optional<int> bin_prec(TokenType type)
{
  switch(type) {
    case TokenType::MULT:
      return 1;
    case TokenType::DIV:
      return 1;
    case TokenType::PLUS:
      return 0;
    case TokenType::SUB:
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
  inline explicit Tokenizer(const std::string& src) 
    : m_src(std::move(src)) {}

  inline std::vector<Token> tokenize() 
  {
    std::vector<Token> tokens;
    std::string buf;

    while (peek().has_value())
    {
      if (std::isalpha(peek().value()))
      {
        buf.push_back(consume());
        while (peek().has_value() && std::isalnum(peek().value()))
        {
          buf.push_back(consume());
        }

        if (buf == "EXIT") /* Built-in function */
        {
          tokens.push_back ({.type = TokenType::EXIT});
          buf.clear();
          continue;
        }

        else if (buf == "set") /* Variable Keyword */
        {
          tokens.push_back ({.type = TokenType::SET});
          buf.clear();
          continue;
        }

        else /* Identifier */
        {
          tokens.push_back({.type = TokenType::ID, .value = buf});
          buf.clear();
          continue;
        }
      }

      else if (std::isdigit(peek().value()))
      {
        buf.push_back(consume());
        while (peek().has_value() && std::isdigit(peek().value()))
          buf.push_back(consume());

        tokens.push_back({.type = TokenType::INT_LIT, .value = buf});
        buf.clear();
        continue;
      }

      else if (peek().value() == '(')
      { 
        consume();
        tokens.push_back({.type = TokenType::LPAREN});
      }
      else if (peek().value() == ')')
      { 
        consume();
        tokens.push_back({.type = TokenType::RPAREN});
      }

      else if (peek().value() == ';')
      {
        tokens.push_back({.type = TokenType::SEMI});
        consume();
        continue;
      }

      else if (peek().value() == '=')
      {
        tokens.push_back({.type = TokenType::EQUALS});
        consume();
        continue;
      }

      else if (peek().value() == '+')
      {
        tokens.push_back({.type = TokenType::PLUS});
        consume();
        continue;
      }
      else if (peek().value() == '*')
      {
        tokens.push_back({.type = TokenType::MULT});
        consume();
        continue;
      }
      else if (peek().value() == '-')
      {
        tokens.push_back({.type = TokenType::SUB});
        consume();
        continue;
      }
      else if (peek().value() == '/')
      {
        tokens.push_back({.type = TokenType::DIV});
        consume();
        continue;
      }

      else if(std::isspace(peek().value()))
      {
        consume();
        continue;
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
  [[nodiscard]] inline std::optional<char> peek(int offset = 0) const 
  {
    if (m_index + offset >= m_src.length())
      return {};
    else
     return m_src.at(m_index + offset);
  }

  inline char consume()
  {
    return m_src.at(m_index++);
  }

  const std::string m_src;
  size_t m_index = 0;
};

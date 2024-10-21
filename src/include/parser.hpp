#pragma once 
#include <vector>
#include <variant>
#include "tokenization.hpp"

struct NodeExprIntLit {
  Token int_lit;
};

struct NodeExprID {
  Token ID;
};

struct NodeExpr {
  std::variant<NodeExprIntLit, NodeExprID> variant;
};

struct NodeStmtExit {
  NodeExpr expr;
};

struct NodeStmtSet {
  Token ID;
  NodeExpr expr;
};

struct NodeStmt {
  std::variant<NodeStmtExit, NodeStmtSet> variant;
};

struct NodeProgram {
  std::vector<NodeStmt> stmts;
};


class Parser
{
public:
  inline explicit Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens)) {}

  std::optional<NodeExpr> parse_expr() 
  {
    if (peek().has_value() && peek().value().type == TokenType::INT_LIT)
    {
      return NodeExpr { .variant = NodeExprIntLit{ .int_lit = consume() } };
    }
    else if (peek().has_value() && peek().value().type == TokenType::ID)
    {
      return NodeExpr { .variant = NodeExprID{ .ID = consume() } };
    }
    else
      return {};
  }

  std::optional<NodeStmt> parse_stmt()
  {
    if (peek().value().type == TokenType::EXIT && peek(1).has_value() 
      && peek(1).value().type == TokenType::LPAREN)
    {
      consume(); /* Consume EXIT */
      consume(); /* Consume LPAREN */

      NodeStmtExit stmt_exit;

      if (auto node_expr = parse_expr())
      {
        stmt_exit = {.expr = node_expr.value()};
      }
      else
      {
        std::cerr << "Invalid expression" << std::endl;
        exit (EXIT_FAILURE);
      }

      if(peek().has_value() && peek().value().type == TokenType::RPAREN)
        consume(); /* Consume RPAREN */
      else
      {
        std::cerr << "Expected `)`" << std::endl;
        exit(EXIT_FAILURE);
      }

      if (peek().has_value() && peek().value().type == TokenType::SEMI)
        consume();
      else 
      {
        std::cerr << "Expected `;`" << std::endl;
        exit (EXIT_FAILURE);
      }
      return NodeStmt { .variant = stmt_exit };
    }
    else if (peek().has_value() && peek().value().type == TokenType::SET  /* SET */
    && peek(1).has_value() && peek(1).value().type == TokenType::ID       /* ID  */
    && peek(2).has_value() && peek(2).value().type == TokenType::EQUALS)  /*  =  */
    {
      consume();  /* consume SET */
      auto stmt_set = NodeStmtSet { .ID = consume() }; /* consume ID */
      consume(); /* consume = */
      if (auto expr = parse_expr())
        stmt_set.expr = expr.value();
      else
      {
        std::cerr << "Invalid expression" << std::endl;
        exit (EXIT_FAILURE);
      }

      if(peek().has_value() && peek().value().type == TokenType::SEMI)
        consume();
      else 
      {
        std::cerr << "Expected `;`" << std::endl;
        exit (EXIT_FAILURE);
      }
      return NodeStmt { .variant = stmt_set };
    }
    else 
    {
      return {};
    }
  }

  std::optional<NodeProgram> parse_program() 
  {
    NodeProgram program;
    while (peek().has_value())
    {
      if (auto stmt = parse_stmt())
        program.stmts.push_back(stmt.value());
      else
      {
        std::cerr << "Invalid statement" << std::endl;
        exit(EXIT_FAILURE);
      }
    }
    return program;
  }


private:
  [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const 
  {
    if (m_index + offset >= m_tokens.size())
      return {};
    else
     return m_tokens.at(m_index + offset);
  }

  inline Token consume()
  {
    return m_tokens.at(m_index++);
  }

  const std::vector<Token> m_tokens;
  size_t m_index = 0;
};

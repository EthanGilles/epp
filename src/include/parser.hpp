#pragma once 
#include <vector>
#include <variant>
#include "arena.hpp"
#include "tokenization.hpp"

struct NodeTermIntLit {
  Token int_lit;
};

struct NodeTermID {
  Token ID;
};

struct NodeExpr;

struct NodeStmtExit {
  NodeExpr *expr;
};

struct NodeBinExprAdd {
  NodeExpr *lhs;
  NodeExpr *rhs;
};

// struct NodeBinExprMulti {
//   NodeExpr *lhs;
//   NodeExpr *rhs;
// };

struct NodeBinExpr {
  NodeBinExprAdd* add;
};

struct NodeTerm {
  std::variant<NodeTermIntLit*, NodeTermID*> variant;
};

struct NodeExpr {
  std::variant<NodeTerm*, NodeBinExpr*> variant;
};

struct NodeStmtSet {
  Token ID;
  NodeExpr* expr;
};

struct NodeStmt {
  std::variant<NodeStmtExit*, NodeStmtSet*> variant;
};

struct NodeProgram {
  std::vector<NodeStmt*> stmts;
};


class Parser
{
public:
  inline explicit Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens))
    , m_allocator(1024 * 1024 * 4)  {} // 4MB 

  std::optional<NodeTerm*> parse_term()
  {
    if (auto int_lit = try_consume(TokenType::INT_LIT))
    {
      auto term_int_lit = m_allocator.alloc<NodeTermIntLit>();
      term_int_lit->int_lit = int_lit.value();

      auto term = m_allocator.alloc<NodeTerm>();
      term->variant = term_int_lit;

      return term;
    }
    else if (auto id = try_consume(TokenType::ID))
    {
      auto term_id = m_allocator.alloc<NodeTermID>();
      term_id->ID = id.value();

      auto term = m_allocator.alloc<NodeTerm>();
      term->variant = term_id;

      return term;
    }
    else 
      return {};
  }

  std::optional<NodeExpr*> parse_expr() 
  {
    if (auto term = parse_term() )
    {
      if (try_consume(TokenType::PLUS).has_value())
      {
        auto bin_expr = m_allocator.alloc<NodeBinExpr>();
        auto bin_expr_add = m_allocator.alloc<NodeBinExprAdd>();
        auto lhs_expr = m_allocator.alloc<NodeExpr>();
        lhs_expr->variant = term.value();
        bin_expr_add->lhs = lhs_expr;
        if(auto rhs = parse_expr())
        {
          bin_expr_add->rhs = rhs.value();
          bin_expr->add = bin_expr_add;
          auto expr = m_allocator.alloc<NodeExpr>();
          expr->variant = bin_expr;
          return expr;
        }
        else
        {
          std::cerr << "Expected expression" << std::endl;
          exit(EXIT_FAILURE);
        }
      }
      else 
      {
        auto expr = m_allocator.alloc<NodeExpr>();
        expr->variant = term.value();
        return expr;
      }
    }
    else 
      return {};
  }

  std::optional<NodeStmt*> parse_stmt()
  {
    /* PARSE EXIT */
    if (peek().value().type == TokenType::EXIT && peek(1).has_value() 
      && peek(1).value().type == TokenType::LPAREN)
    {
      consume(); /* Consume EXIT */
      consume(); /* Consume LPAREN */

      auto stmt_exit = m_allocator.alloc<NodeStmtExit>();

      if (auto node_expr = parse_expr())
      {
        stmt_exit->expr = node_expr.value();
      }
      else
      {
        std::cerr << "Invalid expression" << std::endl;
        exit (EXIT_FAILURE);
      }

      try_consume(TokenType::RPAREN, "Expected `)`");
      try_consume(TokenType::SEMI, "Expected `;`");

      auto stmt = m_allocator.alloc<NodeStmt>();
      stmt->variant = stmt_exit;
      return stmt;

    }
    /* PARSE SET ID = EXPR */
    else if (peek().has_value() && peek().value().type == TokenType::SET  /* SET */
    && peek(1).has_value() && peek(1).value().type == TokenType::ID       /* ID  */
    && peek(2).has_value() && peek(2).value().type == TokenType::EQUALS)  /*  =  */
    {
      consume();  /* consume SET */
      auto stmt_set = m_allocator.alloc<NodeStmtSet>();
      stmt_set->ID = consume(); /* consume ID */
      consume(); /* consume = */
      if (auto expr = parse_expr())
        stmt_set->expr = expr.value();
      else
      {
        std::cerr << "Invalid expression" << std::endl;
        exit (EXIT_FAILURE);
      }

      try_consume(TokenType::SEMI, "Expected `;`");

      auto stmt = m_allocator.alloc<NodeStmt>();
      stmt->variant = stmt_set;
      return stmt;
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

  inline Token try_consume(TokenType type, const std::string& msg)
  {
      if(peek().has_value() && peek().value().type == type)
        return consume();
      else 
      {
        std::cerr << msg << std::endl;
        exit (EXIT_FAILURE);
      }
  }

  inline std::optional<Token> try_consume(TokenType type)
  {
      if(peek().has_value() && peek().value().type == type)
        return consume();
      else 
        return {};
  }

  const std::vector<Token> m_tokens;
  size_t m_index = 0;
  ArenaAllocator m_allocator;
};

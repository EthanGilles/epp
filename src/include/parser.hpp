#pragma once 
#include <cstdlib>
#include <iostream>
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

struct NodeTermParenth {
  NodeExpr *expr;
};

struct NodeStmtExit {
  NodeExpr *expr;
};

struct NodeBinExprAdd {
  NodeExpr *lhs;
  NodeExpr *rhs;
};

struct NodeBinExprMulti {
  NodeExpr *lhs;
  NodeExpr *rhs;
};

struct NodeBinExprSub {
  NodeExpr *lhs;
  NodeExpr *rhs;
};

struct NodeBinExprDiv {
  NodeExpr *lhs;
  NodeExpr *rhs;
};

struct NodeBinExprMod {
  NodeExpr *lhs;
  NodeExpr *rhs;
};

struct NodeBinExpr {
  std::variant<NodeBinExprAdd*, NodeBinExprMulti*, NodeBinExprSub*, NodeBinExprDiv*, NodeBinExprMod* > variant;
};

struct NodeTerm {
  std::variant<NodeTermIntLit*, NodeTermID*, NodeTermParenth* > variant;
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
    else if (auto op = try_consume(TokenType::LPAREN))
    {
      auto expr = parse_expr();
      if (!expr.has_value())
      {
        std::cerr << "Expected expression" << std::endl;
        exit(EXIT_FAILURE);  
      }
      try_consume(TokenType::RPAREN, "Expected `)`");

      auto term_parenth = m_allocator.alloc<NodeTermParenth>();
      term_parenth->expr = expr.value();

      auto term = m_allocator.alloc<NodeTerm>();
      term->variant = term_parenth;
      return term;
    }
    else 
      return {};
  }

  std::optional<NodeExpr*> parse_expr(int min_precedence = 0)
  {
    std::optional<NodeTerm*> term_lhs = parse_term();
    if(!term_lhs.has_value())
      return {};

    auto expr_lhs = m_allocator.alloc<NodeExpr>();
    expr_lhs->variant = term_lhs.value();

    while(true)
    {
      std::optional<Token> curr_token = peek();
      std::optional<int> precedence;
      /* Check precedence */
      if (curr_token.has_value())
      {
        precedence = bin_prec(curr_token->type);
        if (!precedence.has_value() || precedence.value() < min_precedence)
          break;
      }
      else
        break;

      Token op = consume();
      int next_min_precedence = precedence.value() + 1;
      auto expr_rhs = parse_expr(next_min_precedence);
      if (!expr_rhs.has_value())
      {
        std::cerr << "Expected expression following operator" << std::endl;
        exit(EXIT_FAILURE);
      }

      auto expr = m_allocator.alloc<NodeBinExpr>();
      auto expr_lhs2 = m_allocator.alloc<NodeExpr>();

      switch (op.type) {
        case TokenType::MULT: {
          auto multi = m_allocator.alloc<NodeBinExprMulti>();
          expr_lhs2->variant = expr_lhs->variant;
          multi->lhs = expr_lhs2;
          multi->rhs = expr_rhs.value();
          expr->variant = multi;
          break;
        }
        case TokenType::PLUS: {
          auto add = m_allocator.alloc<NodeBinExprAdd>();
          expr_lhs2->variant = expr_lhs->variant;
          add->lhs = expr_lhs2;
          add->rhs = expr_rhs.value();
          expr->variant = add;
          break;
        }
        case TokenType::SUB: {
          auto sub = m_allocator.alloc<NodeBinExprSub>();
          expr_lhs2->variant = expr_lhs->variant;
          sub->lhs = expr_lhs2;
          sub->rhs = expr_rhs.value();
          expr->variant = sub;
          break;
        }
        case TokenType::DIV: {
          auto div = m_allocator.alloc<NodeBinExprDiv>();
          expr_lhs2->variant = expr_lhs->variant;
          div->lhs = expr_lhs2;
          div->rhs = expr_rhs.value();
          expr->variant = div;
          break;
        }
        case TokenType::MOD: {
          auto mod = m_allocator.alloc<NodeBinExprMod>();
          expr_lhs2->variant = expr_lhs->variant;
          mod->lhs = expr_lhs2;
          mod->rhs = expr_rhs.value();
          expr->variant = mod;
          break;
        }
        default: {
          std::cerr << "Invalid operator" << std::endl;
          exit(EXIT_FAILURE);
        }
      }
      expr_lhs->variant = expr;
    }
    return expr_lhs;
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

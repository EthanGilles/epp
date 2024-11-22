#pragma once 
#include <cassert>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <iostream>
#include <variant>
#include <vector>
#include "arena.hpp"
#include "tokenization.hpp"
#include "token-utils.cpp"
#include "grammar.hpp"

class Parser
{
public:
  inline explicit Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens))
    , m_allocator(1024 * 1024 * 4)  {} // 4MB 

  /* TERM -> ID | INT_LIT | (EXPR) | CHAR*/
  std::optional<NodeTerm*> parse_term()
  {
    if (auto int_lit = try_consume(TokenType::INT_LIT))
    {
      auto term_int_lit = m_allocator.emplace<NodeTermIntLit>(int_lit.value());

      auto term = m_allocator.emplace<NodeTerm>(term_int_lit);
      return term;
    }
    else if (auto id = try_consume(TokenType::ID))
    {
      /* PARSE -> ID[EXPR] */
      if(try_consume(TokenType::LBRACKET))
      {
        auto expr = parse_expr();
        try_consume_err(TokenType::RBRACKET);
        auto list = m_allocator.emplace<NodeTermIDLoc>(id.value(), expr.value());
        auto term_id = m_allocator.emplace<NodeTermID>(list);
        auto term = m_allocator.emplace<NodeTerm>(term_id);
        return term;
      }
      /* PARSE -> ID */
      else 
      {
        auto id_lit = m_allocator.emplace<NodeTermIDLit>(id.value());
        auto term_id = m_allocator.emplace<NodeTermID>(id_lit);
        auto term = m_allocator.emplace<NodeTerm>(term_id);
        return term;
      }
    }
    else if (auto op = try_consume(TokenType::LPAREN))
    {
      auto expr = parse_expr();
      if (!expr.has_value())
        error_expected("expression");

      try_consume_err(TokenType::RPAREN);

      auto term_parenth = m_allocator.emplace<NodeTermParenth>(expr.value());

      auto term = m_allocator.emplace<NodeTerm>(term_parenth);
      return term;
    }
    else if (auto character = try_consume(TokenType::CHAR))
    {
      int value = static_cast<int>(character.value().value.value()[0]);
      character.value().value = std::to_string(value);
      auto term_int_lit = m_allocator.emplace<NodeTermIntLit>(character.value());
      auto term = m_allocator.emplace<NodeTerm>(term_int_lit);
      return term;
    }
    else if (auto list = try_consume(TokenType::LEN))
    {
      try_consume_err(TokenType::LPAREN);
      if(auto expr = parse_expr())
      {
        auto term_len = m_allocator.emplace<NodeTermLen>(expr.value());
        auto term = m_allocator.emplace<NodeTerm>(term_len);
        try_consume_err(TokenType::RPAREN);
        return term;
      }
      else if ( auto list_node = parse_list())
      {
        struct ListVisitor {
          Parser &p;
          Token &list;

          NodeTerm* operator()(NodeListPreInit* prelist) {
            Token int_lit = {TokenType::INT_LIT, list.line, std::to_string(prelist->elements.size())};
            auto term_int_lit = p.m_allocator.emplace<NodeTermIntLit>(int_lit);
            auto term = p.m_allocator.emplace<NodeTerm>(term_int_lit);
            auto expr = p.m_allocator.emplace<NodeExpr>(term);

            auto term_len = p.m_allocator.emplace<NodeTermLen>(expr);
            auto term_len_expr = p.m_allocator.emplace<NodeTerm>(term_len);
            return term_len_expr;
          }
          NodeTerm* operator()(NodeListNotInit* prelist) {
            p.error_invalid("list - cannot get the length of an uninitialized list");
            return {};
          }
          NodeTerm* operator()(NodeListRange* prelist) {
            p.error_invalid("list - cannot get the length of an uninitialized list");
            return {};
          }
        };

        ListVisitor lv {.p = *this, .list = list.value()};
        try_consume_err(TokenType::RPAREN);
        return std::visit(lv, list_node.value()->variant);
      }
      else 
        error_expected("expression or list");
    }
    return {};
  }

  std::optional<NodeList*> parse_list() {
    /* PARSE {EXPR, EXPR, ...} */
    if(auto bracket = try_consume(TokenType::LCURLY)) {
      std::vector<NodeExpr*> elements;

      if(auto expr = parse_expr())
        elements.push_back(expr.value());

      while (true)
      {
        if(peek().has_value() && peek().value().type == TokenType::COMMA) 
        {
          if(elements.size() == 0)
            error_expected("expression before comma");
          consume();

          if(auto expr = parse_expr())
            elements.push_back(expr.value());
          else
           error_expected("expression after comma");
        }
        else if (peek().has_value() && peek().value().type == TokenType::RCURLY)
        {
          consume();
          break;
        }
        else 
          error_invalid("List Definition");
      }

      auto pre_init = m_allocator.emplace<NodeListPreInit>(elements);
      auto list = m_allocator.emplace<NodeList>(pre_init);
      return list;
    }
    /* PARSE [EXPR, EXPR] */
    else if(auto bracket = try_consume(TokenType::LBRACKET)) {

      auto lhs = parse_expr();
      if (!lhs.has_value())
        error_expected("expression");

      /* set initial value to 0 */
      Token init_token = {TokenType::INT_LIT, bracket.value().line, "0"};
      auto value = m_allocator.emplace<NodeTermIntLit>(init_token);
      auto term = m_allocator.emplace<NodeTerm>(value);
      std::optional<NodeExpr*> init_value = m_allocator.emplace<NodeExpr>(term);

      /* Two arguments */
      if(try_consume(TokenType::COMMA)) {
        init_value = parse_expr();
        if (!init_value.has_value())
          error_expected("expression following comma");
      }
      if(try_consume(TokenType::RANGE)) {
        auto rhs = parse_expr();
        if (!rhs.has_value())
          error_expected("expression following range");

        try_consume_err(TokenType::RBRACKET);

        auto range = m_allocator.emplace<NodeListRange>(lhs.value(), rhs.value());
        auto list = m_allocator.emplace<NodeList>(range);
        return list;
      }

      try_consume_err(TokenType::RBRACKET);

      auto not_init = m_allocator.emplace<NodeListNotInit>(lhs.value(), init_value.value());
      auto list = m_allocator.emplace<NodeList>(not_init);
      return list;
    }
    else if (auto str = try_consume(TokenType::STRING)) {
      std::vector<NodeExpr*> elements;
      std::string string_lit = str.value().value.value();

      for (char c : string_lit) {
        int ascii_value = static_cast<int>(c);
        Token init_token = {TokenType::INT_LIT, str.value().line, std::to_string(ascii_value)};
        auto c_int = m_allocator.emplace<NodeTermIntLit>(init_token);
        auto c_term = m_allocator.emplace<NodeTerm>(c_int);
        auto c_expr = m_allocator.emplace<NodeExpr>(c_term);
        elements.push_back(c_expr);
      }

      auto pre_init = m_allocator.emplace<NodeListPreInit>(elements);
      auto list = m_allocator.emplace<NodeList>(pre_init);
      return list;
    }
    else if (auto to_string = try_consume(TokenType::TO_STR))
    {
      if (auto expr = parse_expr())
      {
        auto not_init = m_allocator.emplace<NodeListNotInit>(nullptr, expr.value());
        auto list = m_allocator.emplace<NodeList>(not_init);
        return list;
      }
    }
    return {};
  }

  std::optional<NodeExpr*> parse_expr(int min_precedence = 0, bool has_cmp = false)
  {
    std::optional<NodeTerm*> term_lhs = parse_term();
    if(!term_lhs.has_value())
      return {};

    auto expr_lhs = m_allocator.emplace<NodeExpr>(term_lhs.value());

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
      auto expr_rhs = parse_expr(next_min_precedence, has_cmp);
      if (!expr_rhs.has_value())
        error_expected("Expression following an operator");

      auto expr = m_allocator.emplace<NodeBinExpr>();
      auto expr_lhs2 = m_allocator.emplace<NodeExpr>();

      switch (op.type) {
        case TokenType::STAR: {
          expr_lhs2->variant = expr_lhs->variant;
          auto multi = m_allocator.emplace<NodeBinExprMulti>(expr_lhs2, expr_rhs.value());
          expr->variant = multi;
          break;
        }
        case TokenType::PLUS: {
          expr_lhs2->variant = expr_lhs->variant;
          auto add = m_allocator.emplace<NodeBinExprAdd>(expr_lhs2, expr_rhs.value());
          expr->variant = add;
          break;
        }
        case TokenType::MINUS: {
          expr_lhs2->variant = expr_lhs->variant;
          auto sub = m_allocator.emplace<NodeBinExprSub>(expr_lhs2, expr_rhs.value());
          expr->variant = sub;
          break;
        }
        case TokenType::FSLASH: {
          expr_lhs2->variant = expr_lhs->variant;
          auto div = m_allocator.emplace<NodeBinExprDiv>(expr_lhs2, expr_rhs.value());
          expr->variant = div;
          break;
        }
        case TokenType::PERCENT: {
          expr_lhs2->variant = expr_lhs->variant;
          auto mod = m_allocator.emplace<NodeBinExprMod>(expr_lhs2, expr_rhs.value());
          expr->variant = mod;
          break;
        }
        case TokenType::LT: {
          if (has_cmp)
            error_invalid("expression. More than one comparison operator");
          expr_lhs2->variant = expr_lhs->variant;
          auto lt = m_allocator.emplace<NodeBinCmpLT>(expr_lhs2, expr_rhs.value());
          expr->variant = lt;
          has_cmp = true;
          break;
        }
        case TokenType::GT: {
          if (has_cmp)
            error_invalid("expression. More than one comparison operator");
          expr_lhs2->variant = expr_lhs->variant;
          auto gt = m_allocator.emplace<NodeBinCmpGT>(expr_lhs2, expr_rhs.value());
          expr->variant = gt;
          has_cmp = true;
          break;
        }
        case TokenType::LTEQ: {
          if (has_cmp)
            error_invalid("expression. More than one comparison operator");
          expr_lhs2->variant = expr_lhs->variant;
          auto lte = m_allocator.emplace<NodeBinCmpLTEQ>(expr_lhs2, expr_rhs.value());
          expr->variant = lte;
          has_cmp = true;
          break;
        }
        case TokenType::GTEQ: {
          if (has_cmp)
            error_invalid("expression. More than one comparison operator");
          expr_lhs2->variant = expr_lhs->variant;
          auto gte = m_allocator.emplace<NodeBinCmpGTEQ>(expr_lhs2, expr_rhs.value());
          expr->variant = gte;
          has_cmp = true;
          break;
        }
        case TokenType::NOTEQ: {
          if (has_cmp)
            error_invalid("expression. More than one comparison operator");
          expr_lhs2->variant = expr_lhs->variant;
          auto noteq = m_allocator.emplace<NodeBinCmpNOTEQ>(expr_lhs2, expr_rhs.value());
          expr->variant = noteq;
          has_cmp = true;
          break;
        }
        case TokenType::DBEQ: {
          if (has_cmp)
            error_invalid("expression. More than one comparison operator");
          expr_lhs2->variant = expr_lhs->variant;
          auto dbeq = m_allocator.emplace<NodeBinCmpDBEQ>(expr_lhs2, expr_rhs.value());
          expr->variant = dbeq;
          has_cmp = true;
          break;
        }
        default: 
          error_invalid("operator: " + to_string(op.type));
      }
      expr_lhs->variant = expr;
    }
    return expr_lhs;
  }

  std::optional<NodeScope*> parse_scope() {
    if(!try_consume(TokenType::LCURLY).has_value())
      return {};

    auto scope = m_allocator.alloc<NodeScope>();
    int last_line = 0;
    /* Parse statements inside scope */
    while (auto stmt = parse_stmt()) 
      scope->stmts.push_back(stmt.value());

    try_consume_err(TokenType::RCURLY);
    return scope;
  }

  std::optional<NodeIfPred*> parse_if_predicate()
  {
    if (try_consume(TokenType::ELSIF))  {
      try_consume_err(TokenType::LPAREN);
      auto elsif = m_allocator.alloc<NodeIfPredElsif>();

      if(auto expr = parse_expr()) 
        elsif->expr = expr.value();
      else 
        error_expected("expression following `eslif`");

      try_consume_err(TokenType::RPAREN);
      if(auto scope = parse_scope()) 
        elsif->scope = scope.value();
      else
       error_expected("scope following `elsif (expr)`");

      elsif->predicate = parse_if_predicate();
      auto predicate = m_allocator.emplace<NodeIfPred>(elsif);
      return predicate;
    }
    else if (try_consume(TokenType::ELSE)) 
    {
      auto else_ = m_allocator.alloc<NodeIfPredElse>();
      if (auto scope = parse_scope()) 
        else_->scope = scope.value();
      else if (try_consume(TokenType::IF))
        error_invalid("else if statement. Use elsif");
      else
        error_expected("scope following `else`");

      auto predicate = m_allocator.emplace<NodeIfPred>(else_);
      return predicate;
    }
    return {};
  }

  std::optional<NodeStmt*> parse_stmt()
  {

    /* PARSE -> 'please' or 'PLEASE' */
    if( (peek().has_value() && peek().value().type == TokenType::PLEASE) || 
        (peek().has_value() && peek().value().type == TokenType::PLEASE_C) )
    {
      auto please = m_allocator.emplace<NodeStmtPlease>();

      if(peek().value().type == TokenType::PLEASE)
        please->value = 1;
      else
        please->value = 2;

      consume();
      auto stmt = m_allocator.emplace<NodeStmt>(please);
      return stmt;
    }


    /* PARSE -> EXIT */
    else if (peek().has_value() && peek().value().type == TokenType::GOODBYE && 
        peek(1).has_value() && peek(1).value().type == TokenType::LPAREN)
    {
      consume(); /* Consume EXIT */
      consume(); /* Consume LPAREN */

      auto stmt_exit = m_allocator.alloc<NodeStmtExit>();

      if (auto node_expr = parse_expr())
        stmt_exit->expr = node_expr.value();
      else
        error_invalid("expression");

      try_consume_err(TokenType::RPAREN);
      try_consume_err(TokenType::SEMI);

      auto stmt = m_allocator.emplace<NodeStmt>();
      stmt->variant = stmt_exit;
      return stmt;
    }

    /* FIX: INFINITE LOOP WHEN NO CLOSING BRACKET OR BAD LIST DEF */
    /* PARSE -> PRINT (EXPR, EXPR, ...) */
    else if (peek().has_value() && peek().value().type == TokenType::PRINT && 
        peek(1).has_value() && peek(1).value().type == TokenType::LPAREN)
    {
      consume(); /* Consume PRINT */
      consume(); /* Consume LPAREN */
      std::vector<NodeExpr*> args;

      /* PARSE FIRST EXPRESSION / LIST */
      if(auto expr = parse_expr()) 
        args.push_back(expr.value());
      else if (auto list = parse_list())
      {
        /* Variant of not initialized or initialized */
        struct ListVisitor {
          Parser &p;
          std::vector<NodeExpr*> args;

          std::vector<NodeExpr*> operator()(NodeListPreInit* prelist) {
            for(auto expr : prelist->elements)
              args.push_back(expr);
            return args;
          }
          std::vector<NodeExpr*> operator()(NodeListNotInit* prelist) {
            p.error_invalid("print statement - cannot print uninitialized list");
            return {};
          }
          std::vector<NodeExpr*> operator()(NodeListRange* prelist) {
            p.error_invalid("print statement - cannot print uninitialized list");
            return {};
          }
        };

        ListVisitor lv {.p = *this, .args = args};
        args = std::visit(lv, list.value()->variant);
      }
      /* PARSE THE REST OF THE ARGUMENTS */
      while (true)
      {
        if(peek().has_value() && peek().value().type == TokenType::COMMA)
        {
          if(args.size() == 0)
            error_expected("expression before comma");
          else
            consume();

          if(auto expr = parse_expr())
            args.push_back(expr.value());
          else if (auto list = parse_list())
          {
            /* Variant of not initialized or initialized */
            struct ListVisitor {
              Parser &p;
              std::vector<NodeExpr*> args;

              std::vector<NodeExpr*> operator()(NodeListPreInit* prelist) {
                for(auto expr : prelist->elements)
                  args.push_back(expr);
                return args;
              }
              std::vector<NodeExpr*> operator()(NodeListNotInit* prelist) {
                p.error_invalid("list - trying to print uninitialized list");
                return {};
              }
              std::vector<NodeExpr*> operator()(NodeListRange* prelist) {
                p.error_invalid("list - trying to get the length of an uninitialized list");
                return {};
              }
            };

            ListVisitor lv {.p = *this, .args = args};
            args = std::visit(lv, list.value()->variant);
          }
          else
            error_expected("expression after comma");
        }
        else if (peek().has_value() && peek().value().type == TokenType::RPAREN)
        {
          consume();
          break;
        }
        else 
          error_invalid("print statement");
      }

      try_consume_err(TokenType::SEMI);
      auto print_stmt = m_allocator.emplace<NodeStmtPrint>();
      print_stmt->args = args;
      auto stmt = m_allocator.emplace<NodeStmt>(print_stmt);
      return stmt;
    }

    /* PARSE -> PRINTN() */
    else if (peek().has_value() && peek().value().type == TokenType::PRINTNL && 
        peek(1).has_value() && peek(1).value().type == TokenType::LPAREN)
    {
      consume(); /* Consume PRINTN */
      consume(); /* Consume LPAREN */
      std::vector<NodeExpr*> args;

      if(auto expr = parse_expr())
        args.push_back(expr.value());
      else if (auto list = parse_list())
      {
        /* Variant of not initialized or initialized */
        struct ListVisitor {
          Parser &p;
          std::vector<NodeExpr*> args;

          std::vector<NodeExpr*> operator()(NodeListPreInit* prelist) {
            for(auto expr : prelist->elements)
              args.push_back(expr);
            return args;
          }
          std::vector<NodeExpr*> operator()(NodeListNotInit* prelist) {
            p.error_invalid("print statement - cannot print uninitialized list");
            return {};
          }
          std::vector<NodeExpr*> operator()(NodeListRange* prelist) {
            p.error_invalid("print statement - cannot print uninitialized list");
            return {};
          }
        };

        ListVisitor lv {.p = *this, .args = args};
        args = std::visit(lv, list.value()->variant);
      }

      while (true)
      {

        if(peek().has_value() && peek().value().type == TokenType::COMMA)
        {
          if(args.size() == 0)
            error_expected("expression before comma");
          else
            consume();

          if(auto expr = parse_expr())
            args.push_back(expr.value());
          else if (auto list = parse_list())
          {
            /* Variant of not initialized or initialized */
            struct ListVisitor {
              Parser &p;
              std::vector<NodeExpr*> args;

              std::vector<NodeExpr*> operator()(NodeListPreInit* prelist) {
                for(auto expr : prelist->elements)
                  args.push_back(expr);
                return args;
              }
              std::vector<NodeExpr*> operator()(NodeListNotInit* prelist) {
                p.error_invalid("print statement - cannot print uninitialized list");
                return {};
              }
              std::vector<NodeExpr*> operator()(NodeListRange* prelist) {
                p.error_invalid("print statement - cannot print uninitialized list");
                return {};
              }
            };

            ListVisitor lv {.p = *this, .args = args};
            args = std::visit(lv, list.value()->variant);
          }
          else
            error_expected("expression after comma");
        }
        else if (peek().has_value() && peek().value().type == TokenType::RPAREN)
        {
          consume();
          break;
        }
        else 
          error_invalid("printn statement");
      }

      /* add a newline character at the end */
      Token int_lit = {TokenType::INT_LIT, peek(-1).value().line, "10"};
      auto lit = m_allocator.emplace<NodeTermIntLit>(int_lit);
      auto term = m_allocator.emplace<NodeTerm>(lit);
      auto newline = m_allocator.emplace<NodeExpr>(term);
      args.push_back(newline);

      try_consume_err(TokenType::SEMI);

      auto print_stmt = m_allocator.emplace<NodeStmtPrint>();
      print_stmt->args = args;
      auto stmt = m_allocator.emplace<NodeStmt>(print_stmt);
      return stmt;
    }

    /* PARSE -> SET ID = EXPR || [LIST] */
    else if (peek().has_value() && peek().value().type == TokenType::SET  /* SET */
    && peek(1).has_value() && peek(1).value().type == TokenType::ID       /* ID  */
    && peek(2).has_value() && peek(2).value().type == TokenType::EQUALS)  /*  =  */
    {
      consume();  /* consume SET */
      Token ID = consume(); /* consume ID */
      consume(); /* consume = */
      auto stmt_set = m_allocator.alloc<NodeStmtSet>(); 

      if (auto expr = parse_expr()) 
      {
        auto set_id = m_allocator.alloc<NodeStmtSetID>();
        set_id->ID = ID;
        set_id->expr = expr.value();
        stmt_set->variant = set_id;
      }
      else if (auto list = parse_list()) 
      {
        auto set_list = m_allocator.alloc<NodeStmtSetList>();
        set_list->ArrID = ID;
        set_list->list = list.value();
        stmt_set->variant = set_list;
      }
      else 
        error_invalid("expression");

      if(semi)
        try_consume_err(TokenType::SEMI);
      else
        semi = true;

      auto stmt = m_allocator.emplace<NodeStmt>(stmt_set);
      return stmt;
    }


    /* PARSE -> RESET ID = [EXPR] */
    else if(peek().has_value() && peek().value().type == TokenType::RESET && 
            peek(1).has_value() && peek(1).value().type == TokenType::ID && 
            peek(2).has_value() && peek(2).value().type == TokenType::EQUALS )
    {
      consume(); // consume RESET
      const auto reset_id = m_allocator.emplace<NodeStmtResetID>(consume());
      consume(); //consume =

      if(const auto expr = parse_expr()) 
        reset_id->expr = expr.value();
      else 
        error_invalid("expression");

      const auto reset = m_allocator.emplace<NodeStmtReset>(reset_id);

      if(semi)
        try_consume_err(TokenType::SEMI);
      else
        semi = true;

      auto stmt = m_allocator.emplace<NodeStmt>(reset);
      return stmt;
    }

    /* PARSE -> RESET ID += [EXPR] */
    else if(peek().has_value() && peek().value().type == TokenType::RESET && 
            peek(1).has_value() && peek(1).value().type == TokenType::ID && 
            peek(2).has_value() && peek(2).value().type == TokenType::PLUS && 
            peek(3).has_value() && peek(3).value().type == TokenType::EQUALS )
    {
      consume(); // consume RESET
      auto id = consume();
      const auto reset_id = m_allocator.emplace<NodeStmtResetID>(id);
      consume(); //consume +
      consume(); //consume =

      auto id_lit = m_allocator.emplace<NodeTermIDLit>(id);
      auto term_id = m_allocator.emplace<NodeTermID>(id_lit);
      auto term = m_allocator.emplace<NodeTerm>(term_id);
      auto self = m_allocator.emplace<NodeExpr>(term);

      const auto add_expr = parse_expr();
      if(!add_expr.has_value())
        error_invalid("expression");

      // add together itself and the new expression
      auto bin_expr = m_allocator.emplace<NodeBinExprAdd>(self, add_expr.value());
      auto bin = m_allocator.emplace<NodeBinExpr>(bin_expr);
      auto expr = m_allocator.emplace<NodeExpr>(bin);
      // make the reset statement's expression be the addition
      reset_id->expr = expr;

      const auto reset = m_allocator.emplace<NodeStmtReset>(reset_id);

      if(semi)
        try_consume_err(TokenType::SEMI);
      else
        semi = true;

      auto stmt = m_allocator.emplace<NodeStmt>(reset);
      return stmt;
    }

    /* PARSE -> RESET ID -= [EXPR] */
    else if(peek().has_value() && peek().value().type == TokenType::RESET && 
            peek(1).has_value() && peek(1).value().type == TokenType::ID && 
            peek(2).has_value() && peek(2).value().type == TokenType::MINUS && 
            peek(3).has_value() && peek(3).value().type == TokenType::EQUALS )
    {
      consume(); // consume RESET
      auto id = consume();
      const auto reset_id = m_allocator.emplace<NodeStmtResetID>(id);
      consume(); //consume +
      consume(); //consume =

      auto id_lit = m_allocator.emplace<NodeTermIDLit>(id);
      auto term_id = m_allocator.emplace<NodeTermID>(id_lit);
      auto term = m_allocator.emplace<NodeTerm>(term_id);
      auto self = m_allocator.emplace<NodeExpr>(term);

      const auto add_expr = parse_expr();
      if(!add_expr.has_value())
        error_invalid("expression");

      // add together itself and the new expression
      auto bin_expr = m_allocator.emplace<NodeBinExprSub>(self, add_expr.value());
      auto bin = m_allocator.emplace<NodeBinExpr>(bin_expr);
      auto expr = m_allocator.emplace<NodeExpr>(bin);
      // make the reset statement's expression be the addition
      reset_id->expr = expr;

      const auto reset = m_allocator.emplace<NodeStmtReset>(reset_id);

      if(semi)
        try_consume_err(TokenType::SEMI);
      else
        semi = true;

      auto stmt = m_allocator.emplace<NodeStmt>(reset);
      return stmt;
    }

    /* PARSE -> RESET ID *= [EXPR] */
    else if(peek().has_value() && peek().value().type == TokenType::RESET && 
            peek(1).has_value() && peek(1).value().type == TokenType::ID && 
            peek(2).has_value() && peek(2).value().type == TokenType::STAR && 
            peek(3).has_value() && peek(3).value().type == TokenType::EQUALS )
    {
      consume(); // consume RESET
      auto id = consume();
      const auto reset_id = m_allocator.emplace<NodeStmtResetID>(id);
      consume(); //consume +
      consume(); //consume =

      auto id_lit = m_allocator.emplace<NodeTermIDLit>(id);
      auto term_id = m_allocator.emplace<NodeTermID>(id_lit);
      auto term = m_allocator.emplace<NodeTerm>(term_id);
      auto self = m_allocator.emplace<NodeExpr>(term);

      const auto add_expr = parse_expr();
      if(!add_expr.has_value())
        error_invalid("expression");

      // add together itself and the new expression
      auto bin_expr = m_allocator.emplace<NodeBinExprMulti>(self, add_expr.value());
      auto bin = m_allocator.emplace<NodeBinExpr>(bin_expr);
      auto expr = m_allocator.emplace<NodeExpr>(bin);
      // make the reset statement's expression be the addition
      reset_id->expr = expr;

      const auto reset = m_allocator.emplace<NodeStmtReset>(reset_id);

      if(semi)
        try_consume_err(TokenType::SEMI);
      else
        semi = true;

      auto stmt = m_allocator.emplace<NodeStmt>(reset);
      return stmt;
    }

    /* PARSE -> RESET ID /= [EXPR] */
    else if(peek().has_value() && peek().value().type == TokenType::RESET && 
            peek(1).has_value() && peek(1).value().type == TokenType::ID && 
            peek(2).has_value() && peek(2).value().type == TokenType::FSLASH && 
            peek(3).has_value() && peek(3).value().type == TokenType::EQUALS )
    {
      consume(); // consume RESET
      auto id = consume();
      const auto reset_id = m_allocator.emplace<NodeStmtResetID>(id);
      consume(); //consume +
      consume(); //consume =

      auto id_lit = m_allocator.emplace<NodeTermIDLit>(id);
      auto term_id = m_allocator.emplace<NodeTermID>(id_lit);
      auto term = m_allocator.emplace<NodeTerm>(term_id);
      auto self = m_allocator.emplace<NodeExpr>(term);

      const auto add_expr = parse_expr();
      if(!add_expr.has_value())
        error_invalid("expression");

      // add together itself and the new expression
      auto bin_expr = m_allocator.emplace<NodeBinExprDiv>(self, add_expr.value());
      auto bin = m_allocator.emplace<NodeBinExpr>(bin_expr);
      auto expr = m_allocator.emplace<NodeExpr>(bin);
      // make the reset statement's expression be the addition
      reset_id->expr = expr;

      const auto reset = m_allocator.emplace<NodeStmtReset>(reset_id);

      if(semi)
        try_consume_err(TokenType::SEMI);
      else
        semi = true;

      auto stmt = m_allocator.emplace<NodeStmt>(reset);
      return stmt;
    }

    /* PARSE -> RESET ID %= [EXPR] */
    else if(peek().has_value() && peek().value().type == TokenType::RESET && 
            peek(1).has_value() && peek(1).value().type == TokenType::ID && 
            peek(2).has_value() && peek(2).value().type == TokenType::PERCENT && 
            peek(3).has_value() && peek(3).value().type == TokenType::EQUALS )
    {
      consume(); // consume RESET
      auto id = consume();
      const auto reset_id = m_allocator.emplace<NodeStmtResetID>(id);
      consume(); //consume +
      consume(); //consume =

      auto id_lit = m_allocator.emplace<NodeTermIDLit>(id);
      auto term_id = m_allocator.emplace<NodeTermID>(id_lit);
      auto term = m_allocator.emplace<NodeTerm>(term_id);
      auto self = m_allocator.emplace<NodeExpr>(term);

      const auto add_expr = parse_expr();
      if(!add_expr.has_value())
        error_invalid("expression");

      // add together itself and the new expression
      auto bin_expr = m_allocator.emplace<NodeBinExprMod>(self, add_expr.value());
      auto bin = m_allocator.emplace<NodeBinExpr>(bin_expr);
      auto expr = m_allocator.emplace<NodeExpr>(bin);
      // make the reset statement's expression be the addition
      reset_id->expr = expr;

      const auto reset = m_allocator.emplace<NodeStmtReset>(reset_id);

      if(semi)
        try_consume_err(TokenType::SEMI);
      else
        semi = true;

      auto stmt = m_allocator.emplace<NodeStmt>(reset);
      return stmt;
    }

    /* PARSE -> RESET ID[EXPR] = EXPR */
    else if (peek().has_value() && peek().value().type == TokenType::RESET  /* SET */
    && peek(1).has_value() && peek(1).value().type == TokenType::ID       /* ID  */
    && peek(2).has_value() && peek(2).value().type == TokenType::LBRACKET)  /*  [  */
    {
      consume(); //consume SET
      const auto reset_arrid = m_allocator.emplace<NodeStmtResetArrID>(consume());
      const auto reset = m_allocator.emplace<NodeStmtReset>(reset_arrid);
      consume(); //consume [
      
      /* Parse index expression */
      if(const auto expr = parse_expr()) 
        reset_arrid->index = expr.value();
      else 
        error_invalid("expression");

      try_consume_err(TokenType::RBRACKET); //consume ]
      try_consume_err(TokenType::EQUALS); //consume =
      
      /* Parse assignment expression */
      if(const auto expr = parse_expr()) 
        reset_arrid->expr = expr.value();
      else 
        error_invalid("expression");


      if(semi)
        try_consume_err(TokenType::SEMI);
      else
        semi = true;
     
      auto stmt = m_allocator.emplace<NodeStmt>(reset);
      return stmt;
    }

    /* PARSE -> SCOPE */
    else if (peek().has_value() && peek().value().type == TokenType::LCURLY)
    {
      if(auto scope = parse_scope())
      {
        auto stmt = m_allocator.emplace<NodeStmt>(scope.value());
        return stmt;
      }
      else
        error_invalid("scope");
    }

    /* PARSE -> WHILE (EXPR) { STMT } */
    else if (auto while_ = try_consume(TokenType::WHILE))
    {
      try_consume_err(TokenType::LPAREN); 

      auto while_stmt = m_allocator.emplace<NodeStmtWhile>();
      if(const auto expr = parse_expr())
        while_stmt->expr = expr.value();
      else
        error_invalid("expression");

      try_consume_err(TokenType::RPAREN);

      if (const auto scope = parse_scope())
        while_stmt->scope = scope.value();
      else
        error_invalid("scope");

      auto stmt = m_allocator.emplace<NodeStmt>(while_stmt);
      return stmt;
    }

    /* PARSE -> FOR ( SET ID = EXPR ; EXPR ; RESET ID = EXPR) { STMT } */
    else if(auto for_ = try_consume(TokenType::FOR))
    {
      try_consume_err(TokenType::LPAREN);

      auto first_stmt = parse_stmt();
      if(!first_stmt.has_value())
        error_expected("set statement first in for loop");

      auto for_stmt = m_allocator.emplace<NodeStmtFor>(); // create a for loop stmt
      if(std::holds_alternative<NodeStmtSet*>(first_stmt.value()->variant))
      {
        NodeStmtSet* set_stmt = std::get<NodeStmtSet*>(first_stmt.value()->variant);
        if(std::holds_alternative<NodeStmtSetID*>(set_stmt->variant))
          for_stmt->set = set_stmt; 
        else 
          error_invalid("for loop set statement");
      }
      else
        error_invalid("for loop set statement");

      // At this point we have parsed for(set i = 0; 
      if(const auto expr = parse_expr())
        for_stmt->cond = expr.value();
      else
        error_invalid("conditional expression");

      try_consume_err(TokenType::SEMI);

      // Now we have parsed for (i = 0; i < count;  
      semi = false; // TURN SEMI COLONS OFF FOR THIS EXPRESSION
      
      auto second_stmt = parse_stmt();
      if(!second_stmt.has_value())
        error_expected("reset statement after conditional expression");

      if(std::holds_alternative<NodeStmtReset*>(second_stmt.value()->variant))
      {
        NodeStmtReset* reset_stmt = std::get<NodeStmtReset*>(second_stmt.value()->variant);
        if(!std::holds_alternative<NodeStmtResetID*>(reset_stmt->variant))
          error_invalid("for loop reset statement");
      }
      else
        error_invalid("for loop reset statement");

      try_consume_err(TokenType::RPAREN);

      if (const auto scope = parse_scope())
      {
        // ADD THE RESET STATEMENT TO THE END OF THE SCOPE
        scope.value()->stmts.push_back(second_stmt.value());
        for_stmt->scope = scope.value();
      }
      else
        error_invalid("scope");


      auto stmt = m_allocator.emplace<NodeStmt>(for_stmt);
      return stmt;
    }

    /* PARSE -> IF ( EXPR ) { STMT } */
    else if (auto if_ = try_consume(TokenType::IF)) 
    {
      try_consume_err(TokenType::LPAREN);

      auto if_stmt = m_allocator.emplace<NodeStmtIf>();
      if(const auto expr = parse_expr())
        if_stmt->expr = expr.value();
      else
        error_invalid("expression");

      try_consume_err(TokenType::RPAREN);

      if (const auto scope = parse_scope())
        if_stmt->scope = scope.value();
      else
        error_invalid("scope");

      if_stmt->predicate = parse_if_predicate();

      auto stmt = m_allocator.emplace<NodeStmt>(if_stmt);
      return stmt;
    }
    return {};
  }

  std::optional<NodeProgram> parse_program() 
  {
    NodeProgram program;
    while (peek().has_value())
    {
      if (auto stmt = parse_stmt())
        program.stmts.push_back(stmt.value());
      else
        error_invalid("statement");
    }
    return program;
  }


private:
  [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const 
  {
    if (m_index + offset >= m_tokens.size())
      return {};
    return m_tokens.at(m_index + offset);
  }

  Token consume()
  {
    return m_tokens.at(m_index++);
  }


  Token try_consume_err(TokenType type)
  {
    if(peek().has_value() && peek().value().type == type)
      return consume();
    std::string msg = to_string(type) + " but found " + to_string(peek().value().type);
    error_expected(msg);
    return {};
  }

  std::optional<Token> try_consume(TokenType type)
  {
      if(peek().has_value() && peek().value().type == type)
        return consume();
      return {};
  }

  void error_expected(const std::string &msg)
  {
    std::cerr << "[Parser Error] Expected " << msg << " on line " << peek(-1).value().line << std::endl;
    exit(EXIT_FAILURE);
  }

  void error_invalid(const std::string &msg)
  {
    std::cerr << "[Parser Error] Invalid " << msg << " on line " << peek(-1).value().line << std::endl;
    exit(EXIT_FAILURE);
  }

  bool semi = true; 
  const std::vector<Token> m_tokens;
  size_t m_index = 0;
  ArenaAllocator m_allocator;
};

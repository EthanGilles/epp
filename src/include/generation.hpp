#pragma once
#include <cstdlib>
#include <ctime>
#include <variant>
#include <ranges>
#include "grammar.hpp"
#include "parser.hpp"
#include <algorithm>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <cassert>
#include <utility>
#include <vector>


class Generator
{
public:
  inline Generator(NodeProgram root, float max_please, float min_please) 
    : m_program(std::move(root))
    , m_max_please(max_please)
    , m_min_please(min_please){}

  struct Var
  {
    std::string name;
    size_t stack_loc;
    std::vector<int> values;
    bool is_list;
  };

  int gen_term_id(const NodeTermID *term_id)
  {
    struct TermIDVisitor {
      Generator &gen;
      int operator()(const NodeTermIDLit *id_lit) const
      {
        const auto it = std::ranges::find_if(std::as_const(gen.m_vars), [&](const Var& var) 
            { return var.name == id_lit->ID.value.value(); });

        if(it == gen.m_vars.cend())
        {
          std::cerr << "Undeclared identifier: " << id_lit->ID.value.value() << std::endl;
          exit(EXIT_FAILURE);
        }
        if(it->is_list)
        {
          std::cerr << "Trying to access list element with no index value" << std::endl;
          exit(EXIT_FAILURE);
        }

        std::stringstream offset;
        offset << "QWORD [rsp + " << (gen.m_stack_size - (*it).stack_loc - 1) * 8 << "] ; Variable value";
        // std::cout << "accessing: " << it->name << " stack size: " <<  gen.m_stack_size << " location " << it->stack_loc << std::endl;
        gen.push(offset.str());
        return it->values[0];
      }
      int operator()(const NodeTermIDLoc *id_loc) const
      {
        const auto it = std::ranges::find_if(std::as_const(gen.m_vars), [&](const Var& var) 
            { return var.name == id_loc->ID.value.value(); });

        if(it == gen.m_vars.cend())
        {
          std::cerr << "Undeclared identifier: " << id_loc->ID.value.value() << std::endl;
          exit(EXIT_FAILURE);
        }
        if(!it->is_list)
        {
          std::cerr << "Trying to use index on a non-list value" << std::endl;
          exit(EXIT_FAILURE);
        }

        /* time to do asm math to get the index */
        gen.m_output << "    ;; /array location\n";
        int index = gen.gen_expr(id_loc->offset);

        if (index >= it->values.size())
        {
          std::cerr << "Index out of bounds" << std::endl;
          exit(EXIT_FAILURE);
        }

        gen.m_output << "    mov rax, rsp\n";
        gen.m_output << "    add rax, " << (gen.m_stack_size - (*it).stack_loc - 1) * 8 << "\n";

        gen.pop("rbx");
        gen.m_output << "    shl rbx, 3\n";
        gen.m_output << "    sub rax, rbx\n";

        std::stringstream offset;
        offset << "QWORD [rax] ;; index value";
        // std::cout << "accessing: " << it->name << " stack size: " <<  gen.m_stack_size << " location " << it->stack_loc << std::endl;
        gen.push(offset.str());
        return it->values[index];
      }
    };

    TermIDVisitor visitor {.gen = *this};
    return std::visit(visitor, term_id->variant);
  }

  int gen_term(const NodeTerm *term) 
  {
    struct TermVisitor {
      Generator &gen;
      int operator()(const NodeTermIntLit *term_int_lit) const
      {
        gen.m_output << "    mov rax, " << term_int_lit->int_lit.value.value() << "\n";
        gen.push("rax");
        return std::stoi(term_int_lit->int_lit.value.value());
      }
      int operator()(const NodeTermID *term_id) const
      {
        return gen.gen_term_id(term_id);
      }
      int operator()(const NodeTermParenth *term_parenth) const
      {
        return gen.gen_expr(term_parenth->expr);
      }
      int operator()(const NodeTermLen *term_len) const
      {
        if(gen.list_check(term_len->length).has_value())
        {
          size_t list_size = gen.list_check(term_len->length).value().values.size();
          gen.m_output << "    mov rax, " << list_size << "\n";
          gen.push("rax");
          return list_size;
        }
        return gen.gen_expr(term_len->length);
      }
    };

    TermVisitor visitor{.gen = *this};
    return std::visit(visitor, term->variant);
  }

  int gen_bin_expr(const NodeBinExpr *bin_expr)
  {
    struct BinExprVisitor {
      Generator &gen;
      int operator()(const NodeBinExprMulti *multi) const
      {
        int rhs = gen.gen_expr(multi->rhs);
        int lhs = gen.gen_expr(multi->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    mul rbx\n";
        gen.push("rax");
        return lhs * rhs;
      }
      int operator()(const NodeBinExprDiv *div) const 
      {
        int rhs = gen.gen_expr(div->rhs);
        int lhs = gen.gen_expr(div->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    div rbx\n";
        gen.push("rax");
        return lhs / rhs;
      }
      int operator()(const NodeBinExprMod *mod) const
      {
        int rhs = gen.gen_expr(mod->rhs);
        int lhs = gen.gen_expr(mod->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor  rdx, rdx\n"; /* clear rdx */
        gen.m_output << "    div rbx\n";
        gen.push("rdx"); /* rdx has remainder */
        return lhs % rhs;
      }
      int operator()(const NodeBinExprAdd *add) const
      {
        int rhs = gen.gen_expr(add->rhs);
        int lhs = gen.gen_expr(add->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    add rax, rbx\n";
        gen.push("rax");
        return lhs + rhs;
      }
      int operator()(const NodeBinExprSub *sub) const
      {
        int rhs = gen.gen_expr(sub->rhs);
        int lhs = gen.gen_expr(sub->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    sub rax, rbx\n";
        gen.push("rax");
        return lhs - rhs;
      }
      int operator()(const NodeBinCmpGT *gt) const
      {
        int rhs = gen.gen_expr(gt->rhs);
        int lhs = gen.gen_expr(gt->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor rcx, rcx\n";
        gen.m_output << "    cmp rax, rbx\n";
        gen.m_output << "    setg cl\n";
        gen.push("rcx");
        return lhs > rhs ? 1 : 0;
      }
      int operator()(const NodeBinCmpLT *lt) const
      {
        int rhs = gen.gen_expr(lt->rhs);
        int lhs = gen.gen_expr(lt->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor rcx, rcx\n";
        gen.m_output << "    cmp rax, rbx\n";
        gen.m_output << "    setl cl\n";
        gen.push("rcx");
        return lhs < rhs ? 1 : 0;
      }
      int operator()(const NodeBinCmpGTEQ *gteq) const
      {
        int rhs = gen.gen_expr(gteq->rhs);
        int lhs = gen.gen_expr(gteq->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor rcx, rcx\n";
        gen.m_output << "    cmp rax, rbx\n";
        gen.m_output << "    setge cl\n";
        gen.push("rcx");
        return lhs >= rhs ? 1 : 0;
      }
      int operator()(const NodeBinCmpLTEQ *lteq) const
      {
        int rhs = gen.gen_expr(lteq->rhs);
        int lhs = gen.gen_expr(lteq->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor rcx, rcx\n";
        gen.m_output << "    cmp rax, rbx\n";
        gen.m_output << "    setle cl\n";
        gen.push("rcx");
        return lhs <= rhs ? 1 : 0;
      }
      int operator()(const NodeBinCmpNOTEQ *noteq) const
      {
        int rhs = gen.gen_expr(noteq->rhs);
        int lhs = gen.gen_expr(noteq->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor rcx, rcx\n";
        gen.m_output << "    cmp rax, rbx\n";
        gen.m_output << "    setne cl\n";
        gen.push("rcx");
        return lhs != rhs ? 1 : 0;
      }
      int operator()(const NodeBinCmpDBEQ *dbeq) const
      {
        int rhs = gen.gen_expr(dbeq->rhs);
        int lhs = gen.gen_expr(dbeq->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor rcx, rcx\n";
        gen.m_output << "    cmp rax, rbx\n";
        gen.m_output << "    sete cl\n"; 
        gen.push("rcx");
        return lhs == rhs ? 1 : 0;
      }
    };

    BinExprVisitor visitor {.gen = *this};
    return std::visit(visitor, bin_expr->variant);
  }

  int gen_expr(const NodeExpr *expr) 
  {
    struct ExprVisitor
    {
      Generator &gen;

      int operator()(const NodeTerm *term) const
      {
        return gen.gen_term(term);
      }
      int operator()(const NodeBinExpr *bin_expr) const
      {
        return gen.gen_bin_expr(bin_expr);
      }
    };

    ExprVisitor visitor {.gen = *this};
    return std::visit(visitor, expr->variant);
  }

  void gen_scope(const NodeScope *scope) 
  {
    begin_scope();
    for(const NodeStmt *stmt : scope->stmts)
      gen_stmt(stmt);
    end_scope();
  }

  void gen_if_pred(const NodeIfPred* pred, const std::string &end_label) {
    struct PredVisitor {
      Generator &gen;
      const std::string &end_label;
      void operator()(const NodeIfPredElsif *elsif) const
      {
        gen.m_output << "    ;; /elseif\n";
        int bool_val = gen.gen_expr(elsif->expr);
        gen.pop("rax");

        if (bool_val != 0 && bool_val != 1)
          gen.bool_error(std::to_string(bool_val));

        const std::string label = gen.create_label();
        gen.m_output << "    test rax, rax\n";
        gen.m_output << "    jz " << label << "\n";
        gen.gen_scope(elsif->scope);
        gen.m_output << "    jmp " << end_label << "\n";

        if (elsif->predicate.has_value()) {
          gen.m_output << label << ":\n";
          gen.gen_if_pred(elsif->predicate.value(), end_label);
        }

      }
      void operator()(const NodeIfPredElse *else_) const
      {
        gen.m_output << "    ;; /else\n";
        gen.gen_scope(else_->scope);
      }
    };

    PredVisitor visitor {.gen = *this, .end_label = end_label};
    std::visit(visitor, pred->variant);
  }

  void gen_list(const NodeList *list, std::string ID)
  {
    struct ListVisitor {
      Generator &gen;
      std::string &ID;

      void operator()(const NodeListPreInit *pre_init) const
      {
        size_t location = gen.m_stack_size;
        std::vector<int> values;
        gen.m_output << "    ;; /init list\n";
        for(auto element : pre_init->elements)
        {
          int val = gen.gen_expr(element);
          values.push_back(val);
        }
        gen.m_vars.emplace_back(ID, location, values, true);
      }
      void operator()(const NodeListNotInit *not_init) const
      {
        size_t location = gen.m_stack_size;
        std::vector<int> values;

        if (not_init->size != nullptr)
        {


          int size = gen.gen_expr(not_init->size);
          gen.pop("rax"); // don't need size on the stack
          for(int i = 0; i < size; i++)
          {
            int val = gen.gen_expr(not_init->init_value);
            values.push_back(val);
          }
          gen.m_vars.emplace_back(ID, location, values, true);
        }
        else 
        {
          int expr = gen.gen_expr(not_init->init_value);
          gen.pop("rax"); // don't need actual value of expr on the stack
          std::string str_expr = std::to_string(expr);
          for (char digit : str_expr) 
          {
            int val = static_cast<int>(digit);
            gen.m_output << "    mov rax, " << val << "\n";
            gen.push("rax");
            values.push_back(val);
          }
          gen.m_vars.emplace_back(ID, location, values, true);
        }
      }
    };

    ListVisitor visitor {.gen = *this, .ID = ID };
    std::visit(visitor, list->variant);
  }

  void gen_stmt_set_list(const NodeStmtSetList *set_list)
  {
    const auto it = std::ranges::find_if(std::as_const(m_vars), [&](const Var& var) 
        { return var.name == set_list->ArrID.value.value(); });

    if (it != m_vars.cend())
    {
      std::cerr << "Identifier already exists: `" << set_list->ArrID.value.value() << "`" << std::endl;
      exit(EXIT_FAILURE);
    }

    gen_list(set_list->list, set_list->ArrID.value.value());
  }

  void gen_stmt_set(const NodeStmtSet *stmt_set) 
  {
    struct StmtSetVisitor {
      Generator &gen;

      void operator()(const NodeStmtSetID *stmt_id) const
      {
        gen.m_output << "    ;; /set\n";
        const auto it = std::ranges::find_if(std::as_const(gen.m_vars), [&](const Var& var) 
            { return var.name == stmt_id->ID.value.value(); });

        if (it != gen.m_vars.cend())
        {
          std::cerr << "Identifier already exists: `" << stmt_id->ID.value.value() << "`" << std::endl;
          exit(EXIT_FAILURE);
        }
        size_t location = gen.m_stack_size;
        int val = gen.gen_expr(stmt_id->expr);
        std::vector<int> values = {val};
        gen.m_vars.emplace_back(stmt_id->ID.value.value(), location, values, false);
      }
      void operator()(const NodeStmtSetList *list) const
      {
        gen.gen_stmt_set_list(list);
      }
    };

    StmtSetVisitor visitor {.gen = *this };
    std::visit(visitor, stmt_set->variant);
  }

  void gen_stmt_reset(const NodeStmtReset *stmt_reset)
  {
    struct StmtResetVisitor {
      Generator &gen;
      void operator()(const NodeStmtResetID *stmt_id) const
      {
        gen.m_output << "    ;; /reset\n";
        auto it = std::ranges::find_if(gen.m_vars, [&](const Var& var) 
            { return var.name == stmt_id->ID.value.value(); });

        if (it == gen.m_vars.cend())
        {
          std::cerr << "Resetting an undeclared identifier: `" << stmt_id->ID.value.value() << "`" << std::endl;
          exit(EXIT_FAILURE);
        }
        if (it->is_list)
        {
          std::cerr << "Resetting a list variable without an index" << std::endl;
          exit(EXIT_FAILURE);
        }

        int val = gen.gen_expr(stmt_id->expr);
        gen.pop("rax");
        gen.m_output << "    mov [rsp + " << (gen.m_stack_size - it->stack_loc - 1) * 8 << "], rax\n";
        it->values[0] = val;
      }
      void operator()(const NodeStmtResetArrID *arr_id) const
      {
        auto it = std::ranges::find_if(gen.m_vars, [&](const Var& var) 
            { return var.name == arr_id->ArrID.value.value(); });

        if (it == gen.m_vars.cend())
        {
          std::cerr << "Resetting an undeclared identifier: `" << arr_id->ArrID.value.value() << "`" << std::endl;
          exit(EXIT_FAILURE);
        }
        if (!it->is_list)
        {
          std::cerr << "Resetting a non-list variable with an index" << std::endl;
          exit(EXIT_FAILURE);
        }

        gen.m_output << "    ;; /array location\n";
        int expr = gen.gen_expr(arr_id->expr);
        int index = gen.gen_expr(arr_id->index);

        if(index >= it->values.size() || index < 0)
        {
          std::cerr << "Accessed out of bounds index `" << index << "` for list size `" << it->values.size()<< "`" << std::endl;
          exit(EXIT_FAILURE);
        }

        gen.m_output << "    mov rcx, rsp\n";
        gen.m_output << "    add rcx, " << (gen.m_stack_size - (*it).stack_loc - 1) * 8 << "\n";

        gen.pop("rbx"); // pop index
        gen.m_output << "    shl rbx, 3\n";
        gen.m_output << "    sub rcx, rbx\n";
        gen.pop("rax"); // pop value

        gen.m_output << "    mov [rcx], rax\n";
        it->values[index] = expr;
      }
    };

    StmtResetVisitor visitor {.gen = *this};
    std::visit(visitor, stmt_reset->variant);
  }

  std::optional<Var> list_check(const NodeExpr *expr)
  {
    if(std::holds_alternative<NodeTerm*>(expr->variant))
    {
      NodeTerm* term = std::get<NodeTerm*>(expr->variant);
      if(std::holds_alternative<NodeTermID*>(term->variant))
      {
        NodeTermID* term_id = std::get<NodeTermID*>(term->variant);
        if(std::holds_alternative<NodeTermIDLit*>(term_id->variant))
        {
          NodeTermIDLit* id_lit = std::get<NodeTermIDLit*>(term_id->variant);

          const auto it = std::ranges::find_if(std::as_const(m_vars), [&](const Var& var) 
              { return var.name == id_lit->ID.value.value(); });
          
          if(it == m_vars.cend())
          {
            std::cerr << "Undeclared identifier: " << id_lit->ID.value.value() << std::endl;
            exit(EXIT_FAILURE);
          }
          if(it->is_list)
          {
            return *it;
          }
        }
      }
    }
    return {};
  }

  void gen_print_list(const Var& var) 
  {
    for(int i = 0; i < var.values.size(); i++)
    {
      m_output << "    mov rax, [rsp + " << (m_stack_size - var.stack_loc - 1 - i) * 8 << "]\n";
      m_output << "    mov [char], al  ;; Store rax in char\n";
      m_output << "    mov rax, 1\n";
      m_output << "    mov rdi, 1\n";
      m_output << "    mov rdx, 1\n";
      m_output << "    mov rsi, char\n";
      m_output << "    syscall\n";
    }
  }

  void gen_stmt(const NodeStmt *stmt) 
  {
    struct StmtVisitor 
    {
      Generator &gen;
      /* GENERATE -> exit() */
      void operator()(const NodeStmtExit *stmt_exit) const
      {
        gen.m_output << "    ;; /exit\n";
        gen.gen_expr(stmt_exit->expr);
        gen.m_output << "    mov rax, 60  ; Syscall number 60 = exit\n";
        gen.pop("rdi");
        gen.m_output << "    syscall\n";
      }
      /* GENERATE -> print() */
      void operator()(const NodeStmtPrint *stmt_print) const
      {
        std::vector args = stmt_print->args;
        gen.m_output << "    ;; /print\n";
        for(NodeExpr *argument : args) {
          if(auto var = gen.list_check(argument))
          {
            gen.gen_print_list(var.value());
          }
          else
          {
            gen.gen_expr(argument);
            gen.pop("rax");
            gen.m_output << "    mov [char], al  ;; Store rax in char\n";
            gen.m_output << "    mov rax, 1\n";
            gen.m_output << "    mov rdi, 1\n";
            gen.m_output << "    mov rdx, 1\n";
            gen.m_output << "    mov rsi, char\n";
            gen.m_output << "    syscall\n";
          }
        }
      }

      /* GENERATE -> set ID || arrID = expr */
      /* GENERATE -> set arrID = list */
      void operator()(const NodeStmtSet *stmt_set)
      {
        gen.gen_stmt_set(stmt_set);
      }

      /* GENERATE -> reset ID = expr */
      void operator()(const NodeStmtReset *stmt_reset) 
      {
        gen.gen_stmt_reset(stmt_reset);

      }
      /* GENERATE -> { } */
      void operator()(const NodeScope *stmt_scope) 
      {
        gen.gen_scope(stmt_scope);
      }
      /* GENERATE -> if{ } */
      void operator()(const NodeStmtIf *stmt_if)
      {
        gen.m_output << "    ;; /if\n";
        const int bool_val = gen.gen_expr(stmt_if->expr);

        if (bool_val != 0 && bool_val != 1)
          gen.bool_error(std::to_string(bool_val));

        gen.pop("rax");
        const std::string label = gen.create_label();
        gen.m_output << "    test rax, rax\n";
        gen.m_output << "    jz " << label << "\n";
        gen.gen_scope(stmt_if->scope);
        if (stmt_if->predicate.has_value())  
        {
          const std::string end_label = gen.create_label();
          gen.m_output << "    jmp " << end_label << "\n";
          gen.m_output << label << ":\n";
          gen.gen_if_pred(stmt_if->predicate.value(), end_label);
          gen.m_output << end_label << ":\n";
        }
        else
          gen.m_output << label << ":\n";

        gen.m_output << "    ;; /if\n";
      }
      void operator()(const NodeStmtWhile *stmt_while)
      {
        gen.m_output << "    ;; /while\n";

        const std::string start = gen.create_label();
        const std::string end = gen.create_label();

        // gen.m_output << start << ":\n";
        //loop
        int val = gen.gen_expr(stmt_while->expr);

        if (val != 0 && val != 1)
          gen.bool_error(std::to_string(val));

        while(val != 0) 
        {
          gen.pop("rax");
          gen.gen_scope(stmt_while->scope);
          gen.m_stmt_count -= stmt_while->scope->stmts.size();
          val = gen.gen_expr(stmt_while->expr);
        }
        gen.pop("rax");
        gen.m_stmt_count += stmt_while->scope->stmts.size();
        // gen.m_output << "    test rax, rax\n";
        // gen.m_output << "    jz " << end << "\n";
        // gen.m_output << "    jmp " << start << "\n";
        // end 
        // gen.m_output << end << ":\n";
        gen.m_output << "    ;; /while\n";
      }
      /* GENERATE -> please or PLEASE */
      void operator()(const NodeStmtPlease* stmt_pls) 
      {
        gen.m_please_count += stmt_pls->value;
        gen.m_please_stmt += 1;
      }
    };

    m_stmt_count++;
    StmtVisitor visitor {.gen = *this};
    std::visit(visitor, stmt->variant);
  }

  [[nodiscard]] std::string gen_program() 
  {
    m_output << "section .data\n";
    m_output << "    char db 0    ;; Allocate byte for a char\n\n";

    m_output << "section .text\n";
    m_output << "    global _start\n\n";

    m_output << "_start:\n";
    /* Parse prgram */

    for(const NodeStmt *stmt : m_program.stmts)
      gen_stmt(stmt);

    float totalstmts = m_stmt_count - m_please_stmt;
    float polite = m_please_count / totalstmts;

    if (polite < m_min_please) 
      not_polite_msg();
    else if (polite > m_max_please) 
      too_polite_msg();
    
    // For testing please counts.
    // std::cout << "please ratio: " << polite << "\n";
    // std::cout << "please count: " << m_please_count << "\n";
    // std::cout << "total stmts: " << totalstmts << "\n";

    m_output << "    ;; /end\n\n";
    m_output << "    mov rax, 60  ; Syscall number 60 = exit\n";
    m_output << "    xor rdi, rdi ; End program with 0\n";
    m_output << "    syscall";

    return m_output.str();
  }

  void polite_msg(const std::string &msg) 
  {
    m_output.str("");
    m_output << "section .data\n    msg db \"" << msg << "\", 0xA\n";
    m_output << "    msg_len equ $ - msg\n    ;; /set msg and length\n\n";
    m_output << "section .text\n    global _start\n\n_start:\n";
    m_output << "    mov rax, 1\n";
    m_output << "    mov rdi, 1\n";
    m_output << "    mov rsi, msg\n";
    m_output << "    mov rdx, msg_len\n";
    m_output << "    syscall\n";;
  }

  void not_polite_msg() 
  {
    std::srand(std::time(nullptr));
    std::vector<std::string> responses = {
      "Hey, saying 'please' wouldn’t hurt here! 🙏",
      "Looks like someone's in a rush. Try a 'please' this time.",
      "Politeness check: failed! Try saying 'please' next time.",
      "Just one 'please' could make this work, you know?",
      "No 'please'? Compile with some manners next time. 🙏",
      "Hold up – did you forget the magic word?",
      "Try adding a 'please' next time and see if things go smoother, yeah?",
      "I think you were polite enough! Oh wait, no, you weren't. 🙏",
      "Respect your compiler! Saying 'please' might get your program to run.",
      "PLEASE use the magic word next time! 🙏",
      "You haven't been polite enough for the compiler."
    };
    int index = std::rand() % responses.size();
    std::string msg = responses[index];

    m_output.str("");
    m_output << "section .data\n    msg db \"" << msg << "\", 0xA\n";
    m_output << "    msg_len equ $ - msg\n    ;; /set msg and length\n\n";
    m_output << "section .text\n    global _start\n\n_start:\n";
    m_output << "    mov rax, 1\n";
    m_output << "    mov rdi, 1\n";
    m_output << "    mov rsi, msg\n";
    m_output << "    mov rdx, msg_len\n";
    m_output << "    syscall\n";;
  }

  void too_polite_msg() 
  {
    std::srand(std::time(nullptr));
    std::vector<std::string> responses = {
      "Okay, we get it – you're very polite! Tone it down a little.",
      "Respect for the politeness, but maybe hold back a bit?",
      "Whoa, I think the compiler might actually blush at this point!",
      "Alright, no need to butter up the compiler this much.",
      "We get it - you're really polite. Just tone it down.",
      "Hmm, that’s a bit much on the ‘please,’ don’t you think?",
      "You've been far too polite for the compiler.",
    };
    int index = std::rand() % responses.size();
    std::string msg = responses[index];

    m_output.str("");
    m_output << "section .data\n    msg db \"" << msg << "\", 0xA\n";
    m_output << "    msg_len equ $ - msg\n    ;; /set msg and length\n\n";
    m_output << "section .text\n    global _start\n\n_start:\n";
    m_output << "    mov rax, 1\n";
    m_output << "    mov rdi, 1\n";
    m_output << "    mov rsi, msg\n";
    m_output << "    mov rdx, msg_len\n";
    m_output << "    syscall\n";;
  }

private:

  void push(const std::string &reg) 
  {
    m_output << "    push " << reg << "\n";
    m_stack_size++;
  }

  /* Also checks for redundant pushes and removes them */
  void pop(const std::string &reg) 
  {
    std::string output = m_output.str();
    const std::string search = "    push " + reg + "\n";

    /* Worth it? (almost certainly not) Or is it faster to have an extra push and pop instruction? */
    if (output.size() > search.size() && output.rfind(search) == output.size() - search.size()) 
    {
      output.resize(output.size() - search.size());
      m_output.str("");
      m_output << output;
      // m_output << "    ;; push-pop removed " << reg << "\n";
    }
    else
      m_output << "    pop " << reg << "\n";

    // m_output << "    pop " << reg << "\n";
    m_stack_size--;
  }

  void begin_scope() 
  {
    m_scopes.push_back(m_vars.size());
  }

  void end_scope() 
  {
    const size_t pop_count = m_vars.size() - m_scopes.back();
    m_output << "    add rsp, " << pop_count * 8 << "\n";
    m_stack_size -= pop_count;

    for(int i = 0; i < pop_count; i++)
      m_vars.pop_back();

    m_scopes.pop_back();
  }

  std::string create_label() 
  {
    return "label" + std::to_string(m_label_count++);
  }

  void bool_error(const std::string msg)
  {
    std::cerr << "[Visitor] Expected boolean value (0 or 1) but found `" << msg << "`" << std::endl;
    exit(EXIT_FAILURE);
  }

  /* please values */
  float m_please_count = 0;
  float m_please_stmt = 0;
  float m_stmt_count = 0;
  float m_max_please;
  float m_min_please;

  /* program stats */
  const NodeProgram m_program;
  std::stringstream m_output;
  size_t m_stack_size = 0;
  int m_label_count = 0;

  /* program variables */
  std::vector<Var> m_vars {};
  std::vector<size_t> m_scopes;
};

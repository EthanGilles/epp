#pragma once 
#include "parser.hpp"
#include <cstdio>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <cassert>

class Generator
{
public:
  inline Generator(NodeProgram root) 
    : m_program(std::move(root)) {}

  void gen_term(const NodeTerm *term) 
  {
    struct TermVisitor {
      Generator *gen;
      void operator()(const NodeTermIntLit *term_int_lit) const
      {
        gen->m_output << "    mov rax, " << term_int_lit->int_lit.value.value() << "\n";
        gen->push("rax");
      }
      void operator()(const NodeTermID *term_id) const
      {
        if(!gen->m_vars.contains(term_id->ID.value.value()))
        {
          std::cerr << "Unknown identifier `" << term_id->ID.value.value() << "`" << std::endl;
          exit(EXIT_FAILURE);
        }
        const auto &var = gen->m_vars.at(term_id->ID.value.value());
        std::stringstream offset;
        offset << "QWORD [rsp + " << (gen->m_stack_size - var.stack_loc - 1) * 8 << "] ; Variable value\n";
        gen->push(offset.str());
      }
    };

    TermVisitor visitor{.gen = this};
    std::visit(visitor, term->variant);
  }

  void gen_bin_expr(const NodeBinExpr *bin_expr)
  {
    struct BinExprVisitor {
      Generator *gen;
      void operator()(const NodeBinExprAdd *add) const
      {
        gen->gen_expr(add->lhs);
        gen->gen_expr(add->rhs);
        gen->pop("rax");
        gen->pop("rbx");
        gen->m_output << "    add rax, rbx\n";
        gen->push("rax");
      }
      void operator()(const NodeBinExprMulti *multi) const
      {
        gen->gen_expr(multi->lhs);
        gen->gen_expr(multi->rhs);
        gen->pop("rax");
        gen->pop("rbx");
        gen->m_output << "    mul rbx\n";
        gen->push("rax");
      }
    };

    BinExprVisitor visitor {.gen = this};
    std::visit(visitor, bin_expr->variant);
  }

  void gen_expr(const NodeExpr *expr) 
  {
    struct ExprVisitor
    {
      Generator *gen;

      void operator()(const NodeTerm *term) const
      {
        gen->gen_term(term);
      }

      void operator()(const NodeBinExpr *bin_expr) const
      {
        gen->gen_bin_expr(bin_expr);
      }

    };

    ExprVisitor visitor {.gen = this};
    std::visit(visitor, expr->variant);
  }

  void gen_stmt(const NodeStmt *stmt) 
  {
    struct StmtVisitor 
    {
      Generator *gen;
      void operator()(const NodeStmtExit *stmt_exit) const
      {
        gen->gen_expr(stmt_exit->expr);
        gen->m_output << "    mov rax, 60  ; Syscall number 60 = exit\n";
        gen->pop("rdi");
        gen->m_output << "    syscall\n";
      }
      void operator()(const NodeStmtSet *stmt_set)
      {
        if (gen->m_vars.contains(stmt_set->ID.value.value()))
        {
          std::cerr << "Identifier must be unique `" << stmt_set->ID.value.value() << "`" << std::endl;
          exit(EXIT_FAILURE);
        }
        gen->m_vars.insert( {stmt_set->ID.value.value(), Var { .stack_loc = gen->m_stack_size } } );
        gen->gen_expr(stmt_set->expr);
      }
    };

    StmtVisitor visitor {.gen = this};
    std::visit(visitor, stmt->variant);
  }

  [[nodiscard]] std::string gen_program() 
  {
    m_output << "global _start\n_start:\n";
    /* Parse prgram */

    for(const NodeStmt *stmt : m_program.stmts)
      gen_stmt(stmt);

    m_output << "    mov rax, 60  ; Syscall number 60 = exit\n";
    m_output << "    mov rdi, 0   ; End program \n";
    m_output << "    syscall";
    return m_output.str();
  }

private:

  void push(const std::string &reg) 
  {
    m_output << "    push " << reg << "\n";
    m_stack_size++;
  }

  void pop(const std::string &reg) 
  {
    m_output << "    pop " << reg << "\n";
    m_stack_size--;
  }

  struct Var
  {
    size_t stack_loc;
  };
  
  std::unordered_map<std::string, Var> m_vars {};
  const NodeProgram m_program;
  std::stringstream m_output;
  size_t m_stack_size = 0;
};

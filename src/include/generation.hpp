#pragma once 
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
  inline Generator(NodeProgram root) 
    : m_program(std::move(root)) {}

  void gen_term(const NodeTerm *term) 
  {
    struct TermVisitor {
      Generator &gen;
      void operator()(const NodeTermIntLit *term_int_lit) const
      {
        gen.m_output << "    mov rax, " << term_int_lit->int_lit.value.value() << "\n";
        gen.push("rax");
      }
      void operator()(const NodeTermID *term_id) const
      {
        const auto it = std::ranges::find_if(std::as_const(gen.m_vars), [&](const Var& var) 
            { return var.name == term_id->ID.value.value(); });

        if(it == gen.m_vars.cend())
        {
          std::cerr << "Undeclared identifier: " << term_id->ID.value.value() << std::endl;
          exit(EXIT_FAILURE);
        }
        std::stringstream offset;
        offset << "QWORD [rsp + " << (gen.m_stack_size - (*it).stack_loc - 1) * 8 << "] ; Variable value";
        gen.push(offset.str());
      }
      void operator()(const NodeTermParenth *term_parenth) const
      {
        gen.gen_expr(term_parenth->expr);
      }
    };

    TermVisitor visitor{.gen = *this};
    std::visit(visitor, term->variant);
  }

  void gen_bin_expr(const NodeBinExpr *bin_expr)
  {
    struct BinExprVisitor {
      Generator &gen;
      void operator()(const NodeBinExprMulti *multi) const
      {
        gen.gen_expr(multi->rhs);
        gen.gen_expr(multi->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    mul rbx\n";
        gen.push("rax");
      }
      void operator()(const NodeBinExprDiv *div) const 
      {
        gen.gen_expr(div->rhs);
        gen.gen_expr(div->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    div rbx\n";
        gen.push("rax");
      }
      void operator()(const NodeBinExprMod *mod) const
      {
        gen.gen_expr(mod->rhs);
        gen.gen_expr(mod->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor  rdx, rdx\n"; /* clear rdx */
        gen.m_output << "    div rbx\n";
        gen.push("rdx"); /* rdx has remainder */
      }
      void operator()(const NodeBinExprAdd *add) const
      {
        gen.gen_expr(add->rhs);
        gen.gen_expr(add->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    add rax, rbx\n";
        gen.push("rax");
      }
      void operator()(const NodeBinExprSub *sub) const
      {
        gen.gen_expr(sub->rhs);
        gen.gen_expr(sub->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    sub rax, rbx\n";
        gen.push("rax");
      }
    };

    BinExprVisitor visitor {.gen = *this};
    std::visit(visitor, bin_expr->variant);
  }

  void gen_expr(const NodeExpr *expr) 
  {
    struct ExprVisitor
    {
      Generator &gen;

      void operator()(const NodeTerm *term) const
      {
        gen.gen_term(term);
      }
      void operator()(const NodeBinExpr *bin_expr) const
      {
        gen.gen_bin_expr(bin_expr);
      }
    };

    ExprVisitor visitor {.gen = *this};
    std::visit(visitor, expr->variant);
  }

  void gen_scope(const NodeScope *scope) 
  {
    begin_scope();
    for(const NodeStmt *stmt : scope->stmts)
      gen_stmt(stmt);
    end_scope();
  }

  void gen_stmt(const NodeStmt *stmt) 
  {
    struct StmtVisitor 
    {
      Generator &gen;
      void operator()(const NodeStmtExit *stmt_exit) const
      {
        gen.gen_expr(stmt_exit->expr);
        gen.m_output << "    mov rax, 60  ; Syscall number 60 = exit\n";
        gen.pop("rdi");
        gen.m_output << "    syscall\n";
      }
      void operator()(const NodeStmtSet *stmt_set)
      {
        const auto it = std::ranges::find_if(std::as_const(gen.m_vars), [&](const Var& var) 
            { return var.name == stmt_set->ID.value.value(); });

        if (it != gen.m_vars.cend())
        {
          std::cerr << "Identifier already exists: `" << stmt_set->ID.value.value() << "`" << std::endl;
          exit(EXIT_FAILURE);
        }
        gen.m_vars.push_back( {.name = stmt_set->ID.value.value(), .stack_loc = gen.m_stack_size } );
        gen.gen_expr(stmt_set->expr);
      }
      void operator()(const NodeScope *stmt_scope) 
      {
        gen.gen_scope(stmt_scope);
      }
      void operator()(const NodeStmtIf *stmt_if)
      {
        gen.gen_expr(stmt_if->expr);
        gen.pop("rax");
        const std::string label = gen.create_label();
        gen.m_output << "    test rax, rax\n";
        gen.m_output << "    jz " << label << "\n";
        gen.gen_scope(stmt_if->scope);
        gen.m_output << label << ":\n";
      }
    };

    StmtVisitor visitor {.gen = *this};
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

  void begin_scope() 
  {
    m_scopes.push_back(m_vars.size());
  }

  void end_scope() 
  {
    const size_t pop_count = m_vars.size() - m_scopes.back();
    m_output << "    add rsp, " << pop_count * 8 << "\n";
    m_stack_size-= pop_count;

    for(int i = 0; i < pop_count; i++)
      m_vars.pop_back();

    m_scopes.pop_back();
  }

  std::string create_label() 
  {
    return "label" + std::to_string(m_label_count++);
  }

  struct Var
  {
    std::string name;
    size_t stack_loc;
  };
  
  const NodeProgram m_program;
  std::stringstream m_output;
  size_t m_stack_size = 0;
  int m_label_count = 0;

  std::vector<Var> m_vars {};
  std::vector<size_t> m_scopes;
};

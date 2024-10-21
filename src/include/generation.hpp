#pragma once 
#include "parser.hpp"
#include <sstream>
#include <iostream>
#include <unordered_map>

class Generator
{
public:
  inline Generator(NodeProgram root) 
    : m_program(std::move(root)) {}

  void gen_expr(const NodeExpr &expr) 
  {

    struct ExprVisitor
    {
      Generator *gen;

      void operator()(const NodeExprIntLit &expr_int_lit)
      {
        gen->m_output << "    mov rax, " << expr_int_lit.int_lit.value.value() << "\n";
        gen->push("rax");
      }
      void operator()(const NodeExprID &expr_id)
      {
        if(!gen->m_vars.contains(expr_id.ID.value.value()))
        {
          std::cerr << "Unknown identifier `" << expr_id.ID.value.value() << "`" << std::endl;
          exit(EXIT_FAILURE);
        }
        const auto &var = gen->m_vars.at(expr_id.ID.value.value());
        std::stringstream offset;
        offset << "QWORD [rsp + " << (gen->m_stack_size - var.stack_loc - 1) * 8 << "]\n";
        gen->push(offset.str());
      }
    };

    ExprVisitor visitor {.gen = this};
    std::visit(visitor, expr.variant);
  }

  void gen_stmt(const NodeStmt &stmt) 
  {
    struct StmtVisitor 
    {
      Generator *gen;
      void operator()(const NodeStmtExit &stmt_exit) const
      {
        gen->gen_expr(stmt_exit.expr);
        gen->m_output << "    mov rax, 60  ; Syscall number 60 = exit\n";
        gen->pop("rdi");
        gen->m_output << "    syscall\n";
      }
      void operator()(const NodeStmtSet &stmt_set)
      {
        if (gen->m_vars.contains(stmt_set.ID.value.value()))
        {
          std::cerr << "Identifier must be unique `" << stmt_set.ID.value.value() << "`" << std::endl;
          exit(EXIT_FAILURE);
        }
        gen->m_vars.insert( {stmt_set.ID.value.value(), Var { .stack_loc = gen->m_stack_size } } );
        gen->gen_expr(stmt_set.expr);
      }
    };

    StmtVisitor visitor {.gen = this};
    std::visit(visitor, stmt.variant);
  }

  [[nodiscard]] std::string gen_program() 
  {
    m_output << "global _start\n_start:\n";
    /* Parse prgram */

    for(const NodeStmt &stmt : m_program.stmts)
      gen_stmt(stmt);

    m_output << "    mov rax, 60  ; Syscall number 60 = exit\n";
    m_output << "    mov rdi, 0   ; Exit value \n";
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
    m_stack_size++;
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

#pragma once 
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
      void operator()(const NodeBinCmpGT *gt) const
      {
        gen.gen_expr(gt->rhs);
        gen.gen_expr(gt->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor rcx, rcx\n";
        gen.m_output << "    cmp rax, rbx\n";
        gen.m_output << "    setg cl\n";
        gen.push("rcx");
      }
      void operator()(const NodeBinCmpLT *lt) const
      {
        gen.gen_expr(lt->rhs);
        gen.gen_expr(lt->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor rcx, rcx\n";
        gen.m_output << "    cmp rax, rbx\n";
        gen.m_output << "    setl cl\n";
        gen.push("rcx");
      }
      void operator()(const NodeBinCmpGTEQ *gteq) const
      {
        gen.gen_expr(gteq->rhs);
        gen.gen_expr(gteq->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor rcx, rcx\n";
        gen.m_output << "    cmp rax, rbx\n";
        gen.m_output << "    setge cl\n";
        gen.push("rcx");
      }
      void operator()(const NodeBinCmpLTEQ *lteq) const
      {
        gen.gen_expr(lteq->rhs);
        gen.gen_expr(lteq->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor rcx, rcx\n";
        gen.m_output << "    cmp rax, rbx\n";
        gen.m_output << "    setle cl\n";
        gen.push("rcx");
      }
      void operator()(const NodeBinCmpNOTEQ *noteq) const
      {
        gen.gen_expr(noteq->rhs);
        gen.gen_expr(noteq->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor rcx, rcx\n";
        gen.m_output << "    cmp rax, rbx\n";
        gen.m_output << "    setne cl\n";
        gen.push("rcx");
      }
      void operator()(const NodeBinCmpDBEQ *dbeq) const
      {
        gen.gen_expr(dbeq->rhs);
        gen.gen_expr(dbeq->lhs);
        gen.pop("rax");
        gen.pop("rbx");
        gen.m_output << "    xor rcx, rcx\n";
        gen.m_output << "    cmp rax, rbx\n";
        gen.m_output << "    sete cl\n"; 
        gen.push("rcx");
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

  void gen_if_pred(const NodeIfPred* pred, const std::string &end_label) {
    struct PredVisitor {
      Generator &gen;
      const std::string &end_label;
      void operator()(const NodeIfPredElsif *elsif) const
      {
        gen.m_output << "    ;; /elseif\n";
        gen.gen_expr(elsif->expr);
        gen.pop("rax");
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
      /* GENERATE -> set ID = expr */
      void operator()(const NodeStmtSet *stmt_set)
      {
        gen.m_output << "    ;; /set\n";
        const auto it = std::ranges::find_if(std::as_const(gen.m_vars), [&](const Var& var) 
            { return var.name == stmt_set->ID.value.value(); });

        if (it != gen.m_vars.cend())
        {
          std::cerr << "Identifier already exists: `" << stmt_set->ID.value.value() << "`" << std::endl;
          exit(EXIT_FAILURE);
        }
        gen.m_vars.emplace_back(stmt_set->ID.value.value(), gen.m_stack_size);
        gen.gen_expr(stmt_set->expr);
      }
      /* GENERATE -> reset ID = expr */
      void operator()(const NodeStmtReset *stmt_reset) 
      {
        gen.m_output << "    ;; /reset\n";
        auto it = std::ranges::find_if(gen.m_vars, [&](const Var& var) 
            { return var.name == stmt_reset->ID.value.value(); });

        if (it == gen.m_vars.cend())
        {
          std::cerr << "Resetting an undeclared identifier: `" << stmt_reset->ID.value.value() << "`" << std::endl;
          exit(EXIT_FAILURE);
        }
        gen.gen_expr(stmt_reset->expr);
        gen.pop("rax");
        gen.m_output << "    mov [rsp + " << (gen.m_stack_size - it->stack_loc - 1) * 8 << "], rax\n";
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
        gen.gen_expr(stmt_if->expr);
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

        gen.m_output << start << ":\n";
        //loop
        gen.gen_expr(stmt_while->expr);
        gen.pop("rax");
        gen.m_output << "    test rax, rax\n";
        gen.m_output << "    jz " << end << "\n";
        gen.gen_scope(stmt_while->scope);
        gen.m_output << "    jmp " << start << "\n";
        // end 
        gen.m_output << end << ":\n";
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
      polite_msg("You haven't been polite enough for the compiler.");
    else if (polite > m_max_please) 
      polite_msg("You've been way too polite for the compiler.");
    
    // For testing please counts.
    // std::cout << "please ratio: " << polite << "\n";
    // std::cout << "please count: " << m_please_count << "\n";
    // std::cout << "total stmts: " << totalstmts << "\n";

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
    std::string search = "    push " + reg + "\n";

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

  struct Var
  {
    std::string name;
    size_t stack_loc;
    size_t size;
  };

  float m_please_count = 0;
  float m_please_stmt = 0;
  float m_stmt_count = 0;
  
  float m_max_please;
  float m_min_please;
  const NodeProgram m_program;
  std::stringstream m_output;
  size_t m_stack_size = 0;
  int m_label_count = 0;

  std::vector<Var> m_vars {};
  std::vector<size_t> m_scopes;
};

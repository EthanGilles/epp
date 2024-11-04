#pragma once
#include <variant>
#include "tokenization.hpp"

/* INT */
struct NodeTermIntLit {
  Token int_lit;
};

/* ID */
struct NodeTermID {
  Token ID;
};

struct NodeExpr;

/* PARENTHESIS */
struct NodeTermParenth {
  NodeExpr *expr;
};

/* Builtin -> EXIT */
struct NodeStmtExit {
  NodeExpr *expr;
};

/* Builtin -> PRINT */
struct NodeStmtPrint {
  std::vector<NodeExpr*> args;
};


/* BINARY COMPARISONS */
struct NodeBinCmpGT {
  NodeExpr *lhs;
  NodeExpr *rhs;
};

struct NodeBinCmpLT {
  NodeExpr *lhs;
  NodeExpr *rhs;
};

struct NodeBinCmpGTEQ {
  NodeExpr *lhs;
  NodeExpr *rhs;
};

struct NodeBinCmpLTEQ {
  NodeExpr *lhs;
  NodeExpr *rhs;
};

struct NodeBinCmpDBEQ {
  NodeExpr *lhs;
  NodeExpr *rhs;
};

struct NodeBinCmpNOTEQ {
  NodeExpr *lhs;
  NodeExpr *rhs;
};

/* BINARY EXPRESSIONS */
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

/* BIN EXPRESSION */
struct NodeBinExpr {
  std::variant<
  NodeBinExprAdd*, 
  NodeBinExprMulti*, 
  NodeBinExprSub*, 
  NodeBinExprDiv*, 
  NodeBinExprMod*,
  NodeBinCmpLT*,
  NodeBinCmpGT*,
  NodeBinCmpGTEQ*,
  NodeBinCmpLTEQ*,
  NodeBinCmpNOTEQ*,
  NodeBinCmpDBEQ*
  > variant;
};

/* TERM */
struct NodeTerm {
  std::variant<NodeTermIntLit*, NodeTermID*, NodeTermParenth* > variant;
};

/* EXPRESSION */
struct NodeExpr {
  std::variant<NodeTerm*, NodeBinExpr*> variant;
};

/* STMTS */
struct NodeStmtSet {
  Token ID;
  NodeExpr* expr;
};

struct NodeStmt;

/* SCOPE */
struct NodeScope {
  std::vector<NodeStmt*> stmts;
};

/* IF PREDICATE -> ELSIF OR ELSE */
struct NodeIfPred;

struct NodeIfPredElsif {
  NodeExpr* expr;
  NodeScope* scope;
  std::optional<NodeIfPred*> predicate;
};

struct NodeIfPredElse {
  NodeScope* scope;
};

struct NodeIfPred {
  std::variant<NodeIfPredElse*, NodeIfPredElsif*> variant;
};

struct NodeStmtIf {
  NodeExpr* expr;
  NodeScope* scope;
  std::optional<NodeIfPred*> predicate;
};

struct NodeStmtPlease {
  size_t value;
};

struct NodeStmtReset {
  Token ID;
  NodeExpr *expr;
};

/* STMT VARIANTS */
struct NodeStmt {
  std::variant<
  NodeStmtExit*, 
  NodeStmtSet*, 
  NodeStmtPrint*, 
  NodeScope*, 
  NodeStmtIf*, 
  NodeStmtPlease*, 
  NodeStmtReset* > 
  variant;
};

struct NodeProgram {
  std::vector<NodeStmt*> stmts;
};

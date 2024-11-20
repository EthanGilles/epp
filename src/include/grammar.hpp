#pragma once
#include <variant>
#include "tokenization.hpp"

/* INT */
struct NodeTermIntLit {
  Token int_lit;
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

/* ID */
struct NodeTermIDLit {
  Token ID;
};

struct NodeTermIDLoc {
  Token ID;
  NodeExpr* offset;
};

struct NodeTermID {
  std::variant<NodeTermIDLit*, NodeTermIDLoc*> variant;
};




/* STMTS */

struct NodeStmtSetID {
  Token ID;
  NodeExpr* expr;
};


struct NodeListPreInit {
  std::vector<NodeExpr*> elements;
};

struct NodeListNotInit {
  NodeExpr* size;
  NodeExpr* init_value;
};

struct NodeListRange {
  NodeExpr* lhs;
  NodeExpr* rhs;
};

struct NodeList {
  std::variant<NodeListPreInit*, NodeListNotInit*, NodeListRange*> variant;
};

struct NodeTermLen {
  NodeExpr* length;
};

/* TERM */
struct NodeTerm {
  std::variant<NodeTermIntLit*, NodeTermID*, NodeTermParenth*, NodeTermLen* > variant;
};

/* EXPRESSION */
struct NodeExpr {
  std::variant<NodeTerm*, NodeBinExpr*> variant;
};

struct NodeStmtSetList {
  Token ArrID;
  NodeList* list;
};

struct NodeStmtSet {
  std::variant<NodeStmtSetList*, NodeStmtSetID*> variant;
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

struct NodeStmtWhile {
  NodeExpr* expr;
  NodeScope* scope;
};



struct NodeStmtPlease {
  size_t value;
};


struct NodeStmtResetID {
  Token ID;
  NodeExpr *expr;
};

struct NodeStmtResetArrID {
  Token ArrID;
  NodeExpr* index;
  NodeExpr* expr;
};

struct NodeStmtReset {
  std::variant<NodeStmtResetID*, NodeStmtResetArrID*> variant;
};

struct NodeStmtFor {
  NodeStmtSet* set;
  NodeExpr* cond;
  NodeScope* scope;
};

/* STMT VARIANTS */
struct NodeStmt {
  std::variant<
  NodeStmtExit*, 
  NodeStmtSet*, 
  NodeStmtPrint*, 
  NodeScope*, 
  NodeStmtIf*, 
  NodeStmtWhile*, 
  NodeStmtFor*, 
  NodeStmtPlease*, 
  NodeStmtReset* > 
  variant;
};

struct NodeProgram {
  std::vector<NodeStmt*> stmts;
};

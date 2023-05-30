#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "irtree.hpp"

enum ASTtype {
  base,
  comp_unit,
  //..
};
class BaseAST {
 public:
  ASTtype type;
  int id;
  virtual void printTree() = 0;
  virtual std::vector<BaseIr *> buildIrNodes(){};
  virtual BaseIr *buildIrTree(){};
  virtual ~BaseAST() = default;
};

class StartRoot : public BaseAST {
 public:
  std::unique_ptr<BaseAST> comp_unit_ast;
  virtual void printTree() override;
  virtual BaseIr *buildIrTree();
  virtual ~StartRoot() override{};
};

class CompUnitAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_def_ast;
  std::unique_ptr<BaseAST> decl_ast;
  std::unique_ptr<BaseAST> comp_unit_ast;
  virtual void printTree() override;
  virtual ~CompUnitAST() override{};
};

// class DeclAST : public BaseAST {
//  public:
// };

class VarDeclAST : public BaseAST {
 public:
  // if type = 0 , INT type = 1, CHAR
  int type;
  std::unique_ptr<BaseAST> var_def_ast;
  std::unique_ptr<BaseAST> var_def_list_ast;
  virtual void printTree() override;
  virtual ~VarDeclAST() override {}
};

class VarDefListAST : public BaseAST {
 public:
  // if type = 0 , INT type = 1, XXX
  std::unique_ptr<BaseAST> var_def_ast;
  std::unique_ptr<BaseAST> var_def_list_ast;
  virtual void printTree() override;
  virtual ~VarDefListAST() override {}
};

class VarDefAST : public BaseAST {
 public:
  std::string *ident;
  std::unique_ptr<BaseAST> exp_list1_ast;
  std::unique_ptr<BaseAST> initval_ast;
  virtual void printTree() override;
  virtual ~VarDefAST() override {}
};

// type = 0 int
// type = 1 void

class FuncDefAST : public BaseAST {
 public:
  int type;
  std::string *ident;
  std::unique_ptr<BaseAST> func_fparams_ast;
  std::unique_ptr<BaseAST> block_ast;
  virtual BaseIr *buildIrTree();
  void printTree() override;
  virtual ~FuncDefAST() override {}
};

class FuncFParamsAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_fparam_ast;
  std::unique_ptr<BaseAST> func_fparam_list_ast;
  virtual void printTree() override;
  virtual ~FuncFParamsAST() override {}
};

class FuncFParamListAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_fparam_ast;
  std::unique_ptr<BaseAST> func_fparam_list_ast;
  virtual void printTree() override;
  virtual ~FuncFParamListAST() override {}
};

class FuncFParamAST : public BaseAST {
 public:
  // type = 0: int
  // type = 1: int []
  int type;
  std::string *ident;
  std::unique_ptr<BaseAST> exp_list1_ast;
  virtual void printTree() override;
  virtual ~FuncFParamAST() override {}
};

class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> block_item_list_ast;
  virtual void printTree() override;
  virtual BaseIr *buildIrTree();
  virtual ~BlockAST() override {}
};

class BlockItemListAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> block_item_ast;
  std::unique_ptr<BaseAST> block_item_list_ast;
  virtual void printTree() override;
  virtual ~BlockItemListAST() override {}
};

class BlockItemAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> decl_ast;
  std::unique_ptr<BaseAST> stmt_ast;
  virtual BaseIr *buildIrTree();
  virtual std::vector<BaseIr *> buildIrNodes() override;
  virtual void printTree() override;
  virtual ~BlockItemAST() override {}
};
/*
  stmt_rule = 0 : LVal '=' Exp ';'
  stmt_rule = 1 :  ';'
  stmt_rule = 2 :  Exp ';'
  stmt_rule = 3 :  Block
  stmt_rule = 4 :  IF '(' Exp ')' Stmt
  stmt_rule = 5 :  IF '(' Exp ')' Stmt ELSE Stmt
  stmt_rule = 6 :  WHILE '(' Exp ')' Stmt
  stmt_rule = 7 :  BREAK ';'
  stmt_rule = 8 :  CONTINUE ';'
  stmt_rule = 9 :  RETURN ';'
  stmt_rule = 10 :  RETURN Exp ';'
*/
class StmtAST : public BaseAST {
 public:
  int stmt_rule;
  std::unique_ptr<BaseAST> l_val_ast;
  std::unique_ptr<BaseAST> exp_ast;
  std::unique_ptr<BaseAST> block_ast;
  std::unique_ptr<BaseAST> stmt1_ast;
  std::unique_ptr<BaseAST> stmt2_ast;

  virtual std::vector<BaseIr *> buildIrNodes();
  virtual BaseIr *buildIrTree();

  virtual void printTree() override;
  virtual ~StmtAST() override {}
};

class ExpAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> l_or_exp_ast;
  // for const string
  std::string *str;
  // for scanf (&lval)
  std::unique_ptr<BaseAST> l_val;
  int getExpNum();

  virtual BaseIr *buildIrTree();
  virtual void printTree() override;
  virtual ~ExpAST() override {}
};

class LValAST : public BaseAST {
 public:
  std::string *ident;
  std::unique_ptr<BaseAST> exp_list1_ast;

  virtual BaseIr *buildIrTree();
  virtual void printTree() override;
  virtual ~LValAST() override {}
};

class ExpList1AST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp_ast;
  std::unique_ptr<BaseAST> exp_list1_ast;
  virtual void printTree() override;
  virtual ~ExpList1AST() override {}
};

class PrimaryExpAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp_ast;
  std::unique_ptr<BaseAST> l_val_ast;
  std::unique_ptr<BaseAST> number_ast;
  virtual BaseIr *buildIrTree();
  virtual void printTree() override;
  virtual ~PrimaryExpAST() override {}
};

class NumberAST : public BaseAST {
 public:
  int int_const;
  virtual BaseIr *buildIrTree();
  virtual void printTree() override;
  virtual ~NumberAST() override {}
};

class UnaryExpAST : public BaseAST {
 public:
  std::string *ident;
  std::unique_ptr<BaseAST> primary_exp_ast;
  std::unique_ptr<BaseAST> func_rparams_ast;
  std::unique_ptr<BaseAST> unary_op_ast;
  std::unique_ptr<BaseAST> unary_exp_ast;

  virtual BaseIr *buildIrTree();
  virtual void printTree() override;
  virtual ~UnaryExpAST() override {}
};
/*
  unary_op_rule = 0 : '+'
  unary_op_rule = 1 : '-'
  unary_op_rule = 2 : '!'
*/
class UnaryOpAST : public BaseAST {
 public:
  int unary_op_rule;
  virtual void printTree() override;
  virtual ~UnaryOpAST() override {}
};

class FuncRParamsAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp_ast;
  std::unique_ptr<BaseAST> exp_list2_ast;
  virtual void printTree() override;
  virtual ~FuncRParamsAST() override {}
};

class ExpList2AST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp_ast;
  std::unique_ptr<BaseAST> exp_list2_ast;
  virtual void printTree() override;
  virtual ~ExpList2AST() override {}
};

/*
  add_exp_rule = 0 : MulExp
  add_exp_rule = 1 : AddExp '+' MulExp
  add_exp_rule = 2 : AddExp '-' MulExp
*/
class AddExpAST : public BaseAST {
 public:
  int add_exp_rule;
  std::unique_ptr<BaseAST> add_exp_ast;
  std::unique_ptr<BaseAST> mul_exp_ast;
  virtual BaseIr *buildIrTree();
  virtual void printTree() override;
  virtual ~AddExpAST() override {}
};

/*
  mul_exp_rule = 0 : UnaryExp
  mul_exp_rule = 1 : MulExp '*' UnaryExp
  mul_exp_rule = 2 : MulExp '/' UnaryExp
  mul_exp_rule = 3 : MulExp '%' UnaryExp
*/

class MulExpAST : public BaseAST {
 public:
  int mul_exp_rule;
  std::unique_ptr<BaseAST> mul_exp_ast;
  std::unique_ptr<BaseAST> unary_exp_ast;
  virtual BaseIr *buildIrTree();
  virtual void printTree() override;
  virtual ~MulExpAST() override {}
};

/*
  rel_exp_rule = 0 : AddExp
  rel_exp_rule = 1 : RelExp LT AddExp
  rel_exp_rule = 2 : RelExp GT AddExp
  rel_exp_rule = 3 : RelExp LE AddExp
  rel_exp_rule = 4 : RelExp GE AddExp
*/
class RelExpAST : public BaseAST {
 public:
  int rel_exp_rule;
  std::unique_ptr<BaseAST> rel_exp_ast;
  std::unique_ptr<BaseAST> add_exp_ast;
  virtual BaseIr *buildIrTree();
  virtual void printTree() override;
  virtual ~RelExpAST() override {}
};
/*
  rel_exp_rule = 0 : RelExp
  rel_exp_rule = 1 : EqExp EQ RelExp
  rel_exp_rule = 2 : EqExp NE RelExp
*/
class EqExpAST : public BaseAST {
 public:
  int eq_rule;
  std::unique_ptr<BaseAST> eq_exp_ast;
  std::unique_ptr<BaseAST> rel_exp_ast;
  virtual BaseIr *buildIrTree();
  virtual void printTree() override;
  virtual ~EqExpAST() override {}
};
/*
  l_and_exp_rule = 0 : RelExp
  l_and_exp_rule = 1 : EqExp EQ RelExp
*/
class LAndExpAST : public BaseAST {
 public:
  int l_and_exp_rule;
  std::unique_ptr<BaseAST> eq_exp_ast;
  std::unique_ptr<BaseAST> l_and_exp_ast;
  virtual BaseIr *buildIrTree();
  virtual void printTree() override;
  virtual ~LAndExpAST() override {}
};
/*
  l_or_exp_rule = 0 : RelExp
  l_or_exp_rule = 1 : EqExp EQ RelExp
*/
class LOrExpAST : public BaseAST {
 public:
  int l_or_exp_rule;
  std::unique_ptr<BaseAST> l_and_exp_ast;
  std::unique_ptr<BaseAST> l_or_exp_ast;
  virtual BaseIr *buildIrTree();
  virtual void printTree() override;
  virtual ~LOrExpAST() override {}
};

#endif

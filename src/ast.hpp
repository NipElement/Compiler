#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

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
  virtual ~BaseAST() = default;
};

class StartRoot : public BaseAST {
 public:
  std::unique_ptr<BaseAST> comp_unit_ast;
  virtual void printTree() override;
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

class DeclAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> const_decl_ast;
  std::unique_ptr<BaseAST> var_decl_ast;
  virtual void printTree() override;
  virtual ~DeclAST() override {}
};

/*
class ConstDel : public BaseAST {

  virtual void printTree() override;
  virtual ~ConstDel() override {}
};
*/

/*
class BType : public BaseAST {

  virtual void printTree() override;
  virtual ~BType() override {}
};
*/

/*
class ConstDef : public BaseAST {

  virtual void printTree() override;
  virtual ~ConstDef() override {}
};
*/

/*
class ConstInitVal : public BaseAST {

  virtual void printTree() override;
  virtual ~ConstInitVal() override {}
};
*/

/*
class VarDecl : public BaseAST {

  virtual void printTree() override;
  virtual ~VarDecl() override {}
};
*/

/*
class VarDef : public BaseAST {

  virtual void printTree() override;
  virtual ~VarDef() override {}
};
*/

/*
class InitVal : public BaseAST {

  virtual void printTree() override;
  virtual ~InitVal() override {}
};
*/

class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  virtual void printTree() override;
  virtual ~FuncDefAST() override {}
};

class FuncType : public BaseAST {
 public:
  enum type {
    void_,
    int_,
  };
  type type_;
  virtual void printTree() override;
  virtual ~FuncType() override {}
};

/*
class FuncFParams : public BaseAST {

  virtual void printTree() override;
  virtual ~FuncFParams() override {}
};
*/

/*
class FuncFParam : public BaseAST {

  virtual void printTree() override;
  virtual ~FuncFParam() override {}
};
*/

class Block : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;
  virtual void printTree() override;
  virtual ~Block() override {}
};

/*
class BlockItem : public BaseAST {

  virtual void printTree() override;
  virtual ~BlockItem() override {}
};
*/

class Stmt : public BaseAST {
 public:
  std::unique_ptr<BaseAST> number;
  virtual void printTree() override;
  virtual ~Stmt() override {}
};

/*
class Exp : public BaseAST {

  virtual void printTree() override;
  virtual ~Exp() override {}
};
*/

/*
class LVal : public BaseAST {

  virtual void printTree() override;
  virtual ~LVal() override {}
};
*/

/*
class PrimaryExp : public BaseAST {

  virtual void printTree() override;
  virtual ~LVal() override {}
};
*/

class Number : public BaseAST {
 public:
  int content;
  virtual void printTree() override;
  virtual ~Number() override {}
};

/*
class UnaryExp : public BaseAST {

  virtual void printTree() override;
  virtual ~UnaryExp() override {}
};
*/

/*
class UnaryOp : public BaseAST {

  virtual void printTree() override;
  virtual ~UnaryOp() override {}
};
*/
/*
class FuncRParams : public BaseAST {

  virtual void printTree() override;
  virtual ~FuncRParams() override {}
};
*/

/*
class MulExp : public BaseAST {

  virtual void printTree() override;
  virtual ~MulExp() override {}
};
*/

/*
class AddExp : public BaseAST {

  virtual void printTree() override;
  virtual ~AddExp() override {}
};
*/

/*
class RelExp : public BaseAST {

  virtual void printTree() override;
  virtual ~RelExp() override {}
};
*/

/*
class EqExp : public BaseAST {

  virtual void printTree() override;
  virtual ~EqExp() override {}
};
*/

/*
class LAndExp : public BaseAST {

  virtual void printTree() override;
  virtual ~LAndExp() override {}
};
*/

/*
class LOrExp : public BaseAST {

  virtual void printTree() override;
  virtual ~LOrExp() override {}
};
*/

/*
class ConstExp : public BaseAST {

  virtual void printTree() override;
  virtual ~ConstExp() override {}
};
*/
#endif

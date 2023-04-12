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

class CompUnitAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_def;
  virtual void printTree() override;
  virtual ~CompUnitAST() override{};
};

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

class Block : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;
  virtual void printTree() override;
  virtual ~Block() override {}
};

class Stmt : public BaseAST {
 public:
  std::unique_ptr<BaseAST> number;
  virtual void printTree() override;
  virtual ~Stmt() override {}
};

class Number : public BaseAST {
 public:
  int content;
  virtual void printTree() override;
  virtual ~Number() override {}
};
#endif

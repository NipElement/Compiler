#ifndef _IRTREE_H
#define _IRTREE_H
#include <memory>
#include <string>
#include <vector>

enum IrType {
  Root,
  FuncDef,
  Block,
  VarDec,
  Jump,
  Cjump,
  Move,
  Exp,
};

enum ParamType {
  Int,
  Void,
  Arr,
};

class BaseIr {
 public:
  int id;
  IrType type;
  virtual void printTree(){};
  virtual void printLL(){};
};

class RootIr : public BaseIr {
 public:
  std::vector<std::unique_ptr<BaseIr>> funcs;
  virtual void printTree();
  virtual void printLL(){};
};

class BlockIr : public BaseIr {
 public:
  // for print allocate mem for parameter
  int param_num = 0;
  // in fact only Cjump,Exp,Move
  std::vector<std::unique_ptr<BaseIr>> stmts;
  virtual void printTree();
  virtual void printLL(){};
};

class VarDeclIr : public BaseIr {
 public:
  int mem_id;  // in fact $mem_id has the address of this variable
  virtual void printTree();
  virtual void printLL(){};
};

class FuncDefIr : public BaseIr {
 public:
  int type;
  std::vector<ParamType> param_types;
  std::vector<std::string> param_names;
  std::unique_ptr<BaseIr> block;
  std::string name;
  virtual void printTree();
  virtual void printLL(){};
};

// for expression:

enum ExpType {
  Binop,
  Mem,
  Temp,
  Const,
  Call,
};

// BaseIr ExpIr->ExpBase
class ExpIr : public BaseIr {
 public:
  int reg_id;
  ExpType exp_type;
};

enum BinOpType {
  Add,
  Minus,
  Or,
};

class BinopExp : public ExpIr {
 public:
  BinOpType op;
  std::unique_ptr<BaseIr> exp1;
  std::unique_ptr<BaseIr> exp2;
  virtual void printTree();
};

class MemExp : public ExpIr {
 public:
  // id is the address of this lval

  // like a[left]
  // left will be the exp
  //  = sext i32 %(exp->id) to i64
  int signext_id;
  std::unique_ptr<BaseIr> exp;
  virtual void printTree();
};

class TempExp : public ExpIr {
 public:
  // this means the value of a variable will be load into a reg_id
  std::unique_ptr<BaseIr> mem;
  virtual void printTree();
};

class ConstExp : public ExpIr {
 public:
  int value;
  virtual void printTree();
};

class CallExp : public ExpIr {
  enum RetType {
    RetInt,
    RetVoid,
  };

 public:
  virtual void printTree();
  RetType type;
  std::string name;
  std::vector<std::unique_ptr<TempExp>> params;
};

class MoveIr : public BaseIr {
 public:
  // here exp1 could only be mem (left value)
  std::unique_ptr<BaseIr> exp1;
  std::unique_ptr<BaseIr> exp2;
  virtual void printTree();
};

class CjumpIr : public BaseIr {
 public:
  std::unique_ptr<BaseIr> exp;
  std::unique_ptr<BaseIr> t_block;
  std::unique_ptr<BaseIr> f_block;
  int t;
  int f;
  int done;
  virtual void printTree();
};

#endif
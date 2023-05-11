#ifndef _IRTREE_H
#define _IRTREE_H
#include <memory>
#include <string>
#include <vector>

static int ir_id = 0;

enum IrType {
  Root,
  FuncDef,
  Block,
  VarDecl,
  Jump,
  Cjump,
  Move,
  Exp,
  Ret,
  Label,
};

enum VariableType {
  Int,
  Pointer,
  Array,
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
  RootIr() {
    id = ir_id++;
    type = IrType(Root);
  }
  std::vector<std::unique_ptr<BaseIr>> funcs;
  virtual void printTree();
  virtual void printLL();
};

class BlockIr : public BaseIr {
 public:
  BlockIr() {
    id = ir_id++;
    type = IrType(Block);
  }
  // for print allocate mem for parameter
  int param_num = 0;
  // in fact only Cjump,Exp,Move
  std::vector<std::unique_ptr<BaseIr>> stmts;
  virtual void printTree();
  virtual void printLL();
};

class VarDeclIr : public BaseIr {
 public:
  VarDeclIr() {
    id = ir_id++;
    type = IrType(VarDecl);
  }
  VariableType var_type;
  int mem_id;  // in fact $mem_id has the address of this variable

  // size is for array
  int size = -1;

  virtual void printTree();
  virtual void printLL();
};

class FuncDefIr : public BaseIr {
 public:
  enum RetType {
    INT,
    VOID,
  };
  FuncDefIr() {
    id = ir_id++;
    type = IrType(FuncDef);
  }
  RetType ret_type = INT;
  std::vector<VariableType> param_types;
  std::vector<std::string> param_names;
  std::unique_ptr<BaseIr> block;
  std::string name;
  virtual void printTree();
  virtual void printLL();
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
  ExpIr() {
    id = ir_id++;
    type = IrType(Exp);
  }
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
  BinopExp() { exp_type = ExpType(Binop); }
  BinOpType op;
  std::unique_ptr<ExpIr> exp1;
  std::unique_ptr<ExpIr> exp2;
  virtual void printTree();
  virtual void printLL();
};
// 计算值，并把值存在地址reg_id里
class MemExp : public ExpIr {
 public:
  MemExp() { exp_type = ExpType(Mem); }
  VariableType mem_type;
  // for int reg_id is the address of this lval
  // for pointer reg_id is the address of pointer

  // variable below are for pointer, because pointer is a special variable which needs loading from reg_id
  int pointer_value_reg_id;

  // variables below are for array element like a[exp]
  //  = sext i32 %(exp->id) to i64
  int signext_id;
  std::unique_ptr<ExpIr> exp;
  // ele_reg_id is the address of the lval of element in an array
  int ele_reg_id;
  // size is the array size
  int size;
  virtual void printTree();
  virtual void printLL();
};

// is value not location
class TempExp : public ExpIr {
 public:
  // this means the value of a variable will be load into a reg_id
  TempExp() { exp_type = ExpType(Temp); }
  std::unique_ptr<ExpIr> mem;
  virtual void printTree();
  virtual void printLL();
};

// is value not location
class ConstExp : public ExpIr {
 public:
  ConstExp() { exp_type = ExpType(Const); }
  int value;
  virtual void printTree();
  virtual void printLL();
};

class CallExp : public ExpIr {
  enum RetType {
    RetInt,
    RetVoid,
  };

 public:
  CallExp() { exp_type = ExpType(Call); }

  virtual void printTree();
  virtual void printLL();
  RetType type;
  std::string name;
  std::vector<std::unique_ptr<TempExp>> params;
};

class MoveIr : public BaseIr {
 public:
  MoveIr() {
    type = IrType(Move);
    id = ir_id++;
  }
  // here exp1 could only be mem (left value)
  std::unique_ptr<ExpIr> exp1;
  std::unique_ptr<ExpIr> exp2;
  virtual void printTree();
  virtual void printLL();
};

class CjumpIr : public BaseIr {
 public:
  CjumpIr() {
    type = IrType(Cjump);
    id = ir_id++;
  }
  std::unique_ptr<ExpIr> exp;
  std::unique_ptr<BaseIr> t_block;
  std::unique_ptr<BaseIr> f_block;
  int condition_reg;
  int t;
  int f;
  int done;
  virtual void printTree();
  virtual void printLL();
};

class RetIr : public BaseIr {
  // if ret_value==nullptr, that is return void
 public:
  RetIr() {
    type = IrType(Ret);
    id = ir_id++;
  }
  std::unique_ptr<ExpIr> ret_value;
  virtual void printTree();
};
class JumpIr : public BaseIr {
 public:
  JumpIr() {
    type = IrType(Jump);
    id = ir_id++;
  }
  int label;
  virtual void printTree();
};
class LabelIr : public BaseIr {
 public:
  LabelIr() {
    type = IrType(Label);
    id = ir_id++;
  }
  int label;
  virtual void printTree();
};
#endif
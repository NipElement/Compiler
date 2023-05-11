#include <iostream>
#include "irtree.hpp"

using namespace std;

int attr_id = 0;
void RootIr::printLL() {
  // omit some attributes
  for (int i = funcs.size() - 1; i >= 0; i--) {
    funcs[i]->printLL();
  }
}

void BlockIr::printLL() {
  // print
  for (int i = 0; i < stmts.size(); i++) {
    stmts[i]->printLL();
  }
}

void VarDeclIr::printLL() {
  if (var_type == VariableType(Int)) {
    cout << "  %" << mem_id << " = alloca i32, align 4" << endl;
  } else if (var_type == VariableType(Array)) {
    //%5 = alloca [20 x i32], align 16
    std::string array_type = "[" + to_string(size) + " x i32]";
    cout << "  %" << mem_id << " = alloca " << array_type << ", align 16" << endl;
  }
}

void FuncDefIr::printLL() {
  std::cout << "define ";
  if (type == 0) {
    std::cout << "i32 ";
  } else {
    std::cout << "void ";
  }
  std::cout << "@" << name << "(";
  for (int i = 0; i < param_names.size(); i++) {
    if (i != 0) {
      std::cout << ", ";
    }
    if (param_types[i] == VariableType(Int)) {  // int
      std::cout << "i32 ";
    } else if (param_types[i] == VariableType(Pointer)) {  // int*
      std::cout << "i32* ";
    } else {
      // ..
    }
    cout << "noundef ";
    std::cout << "%" << i;
  }
  std::cout << ")";

  std::cout << " #" << attr_id << " ";
  std::cout << "{" << std::endl;

  for (int i = 0; i < param_names.size(); i++) {
    // print
    if (param_types[i] == VariableType(Int)) {
      cout << "  %" << param_names.size() + i + 1 << " = alloca i32, align 4" << endl;
      // store i32 %i, i32* %param_names.size() + i + 1, align 4
      cout << "  store i32 "
           << "%" << i << ", i32* %" << param_names.size() + i + 1 << ", align 4" << endl;
    } else if (param_types[i] == VariableType(Pointer)) {
      cout << "  %" << param_names.size() + i + 1 << " = alloca i32*, align 8" << endl;
      // store i32 %i, i32* %param_names.size() + i + 1, align 4
      cout << "  store i32* "
           << "%" << i << ", i32** %" << param_names.size() + i + 1 << ", align 8" << endl;
    }
  }

  block->printLL();

  std::cout << "}" << std::endl;
  cout << endl;
}

void BinopExp::printLL() {
  // before
  if (exp1->exp_type != Const) {
    exp1->printLL();
  }
  if (exp2->exp_type != Const) {
    exp2->printLL();
  }
  int constFlag = 0;
  constFlag = (exp1->exp_type == Const) + (exp2->exp_type == Const);

  if (op == Add) {
    if (constFlag == 0) {
      cout << "  %" << reg_id << " = "
           << "add nsw i32 "
           << "%" << exp1->reg_id << ", %" << exp2->reg_id << endl;
    } else if (exp1->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp1.get());
      // %reg_id = add nsw i32 theConstExp->value, %exp2->reg_id
      cout << "  %" << reg_id << " = "
           << "add nsw i32 " << theConstExp->value << ", %" << exp2->reg_id << endl;
    } else if (exp2->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp2.get());
      // %reg_id = add nsw i32 theConstExp->value, %exp1->reg_id
      cout << "  %" << reg_id << " = "
           << "add nsw i32 " << theConstExp->value << ", %" << exp1->reg_id << endl;
    }

  } else if (op == Minus) {
    if (constFlag == 0) {
      cout << "  %" << reg_id << " = "
           << "sub nsw i32 "
           << "%" << exp1->reg_id << ", %" << exp2->reg_id << endl;
    } else if (exp1->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp1.get());
      // %reg_id = sub nsw i32 theConstExp->value, %exp2->reg_id
      cout << "  %" << reg_id << " = "
           << "sub nsw i32 " << theConstExp->value << ", %" << exp2->reg_id << endl;
    } else if (exp2->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp2.get());
      // %reg_id = sub nsw i32 %exp1->reg_id, theConstExp->value
      cout << "  %" << reg_id << " = "
           << "sub nsw i32 "
           << "%" << exp1->reg_id << ", " << theConstExp->value << endl;
    }
  } else if (op == Or) {
  } else if (op == Ge) {
    if (constFlag == 0) {
      // %bool_result_reg = icmp sge i32 %exp1->reg_id, %exp2->reg_id
      // %reg_id = zext i1 %10 to i32
      cout << "  %" << bool_result_reg << " = icmp sge i32 % " << exp1->reg_id << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << reg_id << " to i32" << endl;
    } else if (exp1->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp1.get());
      // %bool_result_reg = icmp sge i32 const, %exp2->reg_id
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp sge i32 " << theConstExp->value << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    } else if (exp2->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp2.get());
      // %bool_result_reg = icmp sge i32 %exp2->reg_id, const
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp sge i32 %" << exp2->reg_id << ", " << theConstExp->value << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    }
  } else if (op == Lt) {
    if (constFlag == 0) {
      // %bool_result_reg = icmp sge i32 %exp1->reg_id, %exp2->reg_id
      // %reg_id = zext i1 %10 to i32
      cout << "  %" << bool_result_reg << " = icmp slt i32 % " << exp1->reg_id << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << reg_id << " to i32" << endl;
    } else if (exp1->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp1.get());
      // %bool_result_reg = icmp sge i32 const, %exp2->reg_id
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp slt i32 " << theConstExp->value << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    } else if (exp2->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp2.get());
      // %bool_result_reg = icmp sge i32 %exp2->reg_id, const
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp slt i32 %" << exp2->reg_id << ", " << theConstExp->value << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    }
  }
}

void MemExp::printLL() {
  // if (exp) {
  //   // as left value, store
  //   exp->printLL();
  //   // store i32 %exp->reg_id, i32* %reg_id, align 4
  //   cout << "  store i32 %" << exp->reg_id << ", i32* %" << reg_id << ", align 4" << endl;
  // } else {
  //   // as right value, load and here we do not need to do anything
  // }

  if (exp == nullptr) {
    // simple variable int, do nothing
  } else if (mem_type == VariableType(Array)) {
    // array element as left value: like a[exp]=..
    exp->printLL();
    // sext i32 %12 to i64
    cout << "  %" << signext_id << " = sext i32 %" << exp->reg_id << " to i64" << endl;
    // %10 = getelementptr inbounds [10 x i32], [10 x i32]* %3, i64 0, i64 %9
    std::string array_type = "[" + to_string(size) + " x i32]";

    cout << "  %" << ele_reg_id << " = getelementptr inbounds ";
    cout << array_type << ", " << array_type << "*"
         << " %" << reg_id << ", i64 0, i64 %" << signext_id << std::endl;
  } else if (mem_type == VariableType(Pointer)) {
    std::string pointer_type = "i32*";
    std::string pointed_type = "i32";
    // load pointer value from memory
    cout << "  %" << pointer_value_reg_id << " = load " << pointer_type << ", " << pointer_type << "*"
         << " %" << reg_id << ", align 8" << endl;
    // pointer with [] as left value, compute the exp
    exp->printLL();
    //%9 = sext i32 %8 to i64
    cout << "  %" << signext_id << " = sext i32 %" << exp->reg_id << " to i64" << endl;
    // %8 = getelementptr inbounds i32, i32* %7, i64 1

    cout << "  %" << ele_reg_id << " = getelementptr inbounds " << pointed_type << ", " << pointer_type << "%"
         << pointer_value_reg_id << ", "
         << "i64 %" << signext_id << endl;
  }
}

void TempExp::printLL() {
  // calculate the location
  mem->printLL();

  auto mem_exp = dynamic_cast<MemExp *>(mem.get());
  if (mem_exp && mem_exp->exp != nullptr) {  // array element or pointer [] element
    cout << "  %" << reg_id << " = load i32, i32* %" << mem_exp->ele_reg_id << ", align 4" << endl;
  } else {
    // %10 = load i32, i32* %6, align 4
    cout << "  %" << reg_id << " = load i32, i32* %" << mem->reg_id << ", align 4" << endl;
  }
}

void ConstExp::printLL() {
  // store i32 value, i32* %reg_id, align 4
  cout << "  store i32 " << value << ", i32 * %" << reg_id << ", align 4" << endl;
}

void CallExp::printLL() {}

void MoveIr::printLL() {
  // Value
  exp2->printLL();
  // Mem
  exp1->printLL();
  // store i32 %exp2->reg_id, i32* %exp1->reg_id, align 4
  if (dynamic_cast<MemExp *>(exp1.get())->mem_type == VariableType(Int)) {  // simple variable int
    cout << "  store i32 %" << exp2->reg_id << ", i32* %" << exp1->reg_id << ", align 4" << endl;
  } else {  // array or pointer
    cout << "  store i32 %" << exp2->reg_id << ", i32* %" << dynamic_cast<MemExp *>(exp1.get())->ele_reg_id
         << ", align 4 " << endl;
  }
}

void CjumpIr::printLL() {
  // produce 3 labels
  exp->printLL();

  // %condition_reg = icmp ne i32 %exp->reg_id, 0
  cout << "  %" << condition_reg << " = icmp ne i32 %" << exp->reg_id << ", 0" << endl;
  // br i1 %condition_reg, label %t, label %f
  cout << "  br i1 %" << condition_reg << ", label %" << t << ", label %" << f << endl;
  // t:
  cout << endl;
  cout << t << ":" << endl;
  t_block->printLL();
  cout << "  br label %" << done << endl;
  // f:
  cout << endl;
  cout << f << ":" << endl;
  f_block->printLL();
  cout << "  br label %" << done << endl;
  // done:
  cout << endl;
  cout << done << ":" << endl;
}
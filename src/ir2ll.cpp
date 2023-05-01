#include <iostream>
#include "irtree.hpp"

using namespace std;

int attr_id = 0;
void RootIr::printLL() {
  // omit some attributes
  for (int i = 0; i < funcs.size(); i++) {
    funcs[i]->printLL();
  }
}

void BlockIr::printLL() {
  // print
  for (int i = 0; i < stmts.size(); i++) {
    stmts[i]->printLL();
  }
}

void VarDeclIr::printLL() { cout << "%" << mem_id << " = alloca i32, align 4" << endl; }

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
    if (param_types[i] == ParamType(Int)) {
      std::cout << "i32 ";
    } else {
      // ...
    }
    cout << "noundef ";
    std::cout << "%" << i;
  }
  std::cout << ")";

  std::cout << " #" << attr_id << " ";
  std::cout << "{" << std::endl;

  block->printLL();

  std::cout << "}" << std::endl;
}

void BinopExp::printLL() {
  // before
  if (op == Add) {
    cout << "%" << reg_id << "add nsw i32 "
         << "%" << exp1->reg_id << ", %" << exp2->reg_id << endl;
  }
}
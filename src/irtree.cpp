#include "irtree.hpp"
#include <iostream>
using namespace std;

void RootIr::printTree() {
  std::cout << "digraph G{ " << std::endl;
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "StartRoot\\nid=" << id;
  std::cout << "\"];" << std::endl;

  for (int i = 0; i < funcs.size(); i++) {
    funcs[i]->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << funcs[i]->id << ";" << std::endl;
  }
  std::cout << "}" << std::endl;
}

void FuncDefIr::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "FuncDefIr\\nid=" << id;
  std::cout << "\\n";
  std::cout << name;
  std::cout << "\\n";
  for (auto el : param_types) {
    std::cout << el << " ";
  }
  std::cout << "\\n";
  for (auto el : param_names) {
    std::cout << el << " ";
  }
  std::cout << "\"];" << std::endl;

  if (block) {
    block->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << block->id << ";" << std::endl;
  }
}

void BlockIr::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "BlockIr\\nid=" << id;
  std::cout << "\\n";
  std::cout << "\"];" << std::endl;
  for (int i = 0; i < stmts.size(); i++) {
    stmts[i]->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << stmts[i]->id << ";" << std::endl;
  }
}

void VarDeclIr::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "VarDeclIr\\nid=" << id;
  std::cout << "\\n";
  std::cout << "mem_id=" << mem_id;
  std::cout << "\"];" << std::endl;
}

void MoveIr::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "MoveIr\\nid=" << id;
  std::cout << "\\n";
  std::cout << "\"];" << std::endl;

  if (exp1) {
    exp1->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp1->id << ";" << std::endl;
  }

  if (exp2) {
    exp2->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp2->id << ";" << std::endl;
  }
}

void CjumpIr::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "CjumpIr\\nid=" << id;
  std::cout << "\\n";
  std::cout << "t=" << t << " "
            << "f=" << f << "\\n";
  std::cout << "done=" << done << "\\n";
  std::cout << "\"];" << std::endl;

  if (exp) {
    exp->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp->id << ";" << std::endl;
  }

  if (t_block) {
    t_block->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << t_block->id << ";" << std::endl;
  }

  if (f_block) {
    f_block->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << f_block->id << ";" << std::endl;
  }
}

void ConstExp::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "ConstIr\\nid=" << id;
  std::cout << "\\n";
  if (const_type == ConstType(CInt)) {
    std::cout << "int value=" << value;
  } else if (const_type == ConstType(CStr)) {
    std::cout << "str value=" << str;
  }
  std::cout << "\\nreg_id=" << reg_id;
  std::cout << "\"];" << std::endl;
}

void MemExp::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "MemIr\\nid=" << id;
  std::cout << "\\n";
  std::string type = "";
  switch (mem_type) {
    case VariableType(Int):
      type = "int";
      break;
    case VariableType(Array):
      type = "array";
      break;
    case VariableType(CharArray):
      type = "char array";
      break;
    case VariableType(CharPtrArray):
      type = "Char Ptr array";
      break;
    case VariableType(Pointer):
      type = "pointer";
      break;
    case VariableType(CharPointer):
      type = "Char Ptr ";
      break;
    default:
      break;
  }
  std::cout << "type=" << type;

  if (exp) {  // array or pointer
    if (mem_type == VariableType(Array) || mem_type == VariableType(Char) || VariableType(CharPtrArray)) {  // array
      std::cout << "\\narray=$" << array_reg_id;
      std::cout << "\\n element=$" << reg_id;
      std::cout << "\\n signext=$" << signext_id;
      std::cout << "\"];" << std::endl;
      exp->printTree();
      std::cout << "SYNTAX_NODE_" << id << "  ->  "
                << "SYNTAX_NODE_" << exp->id << ";" << std::endl;
    } else if (mem_type == VariableType(Pointer) || mem_type == VariableType(CharPointer)) {
      std::cout << "array=$" << array_reg_id;
      std::cout << "\\n element=$" << reg_id;
      std::cout << "\\n signext=$" << signext_id;
      std::cout << "\\n pointer_value_reg_id=" << pointer_value_reg_id;
      std::cout << "\"];" << std::endl;
      exp->printTree();
      std::cout << "SYNTAX_NODE_" << id << "  ->  "
                << "SYNTAX_NODE_" << exp->id << ";" << std::endl;
    }
  } else {
    // simple variable
    std::cout << "\\nsimple reg_id=$" << reg_id;
    std::cout << "\"];" << std::endl;
  }
}

void TempExp::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "TempIr\\nid=" << id;
  std::cout << "\\n";
  std::cout << "reg_id=$" << reg_id;
  std::cout << "\"];" << std::endl;
  if (mem) {
    mem->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << mem->id << ";" << std::endl;
  }
}

void BinopExp::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "BinopIr\\nid=" << id;
  std::cout << "\\noptype=" << op;
  std::cout << "\\nreg_id=$" << reg_id;
  std::cout << "\"];" << std::endl;
  if (exp1) {
    exp1->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp1->id << ";" << std::endl;
  }

  if (exp2) {
    exp2->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp2->id << ";" << std::endl;
  }
}

void NorBoolBinopExp::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "BinopIr\\nid=" << id;
  std::cout << "\\noptype=" << op;
  std::cout << "\\nreg_id=$" << reg_id;
  std::cout << "\"];" << std::endl;
  if (exp1) {
    exp1->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp1->id << ";" << std::endl;
  }

  if (exp2) {
    exp2->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp2->id << ";" << std::endl;
  }
}

void AndOrBinopExp::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "BinopIr\\nid=" << id;
  std::cout << "\\noptype=" << op;
  std::cout << "\\nreg_id=$" << reg_id;
  std::cout << "\"];" << std::endl;
  if (exp1) {
    exp1->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp1->id << ";" << std::endl;
  }

  if (exp2) {
    exp2->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp2->id << ";" << std::endl;
  }
}

void JumpIr::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "JumpIr\\nid=" << id;
  std::cout << "\\njump to label " << label;
  std::cout << "\"];" << std::endl;
}

void LabelIr::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "LabelIr\\nid=" << id;
  std::cout << "\\nlabel=" << label;
  std::cout << "\"];" << std::endl;
}

void RetIr::printTree() {
  // now only support return void
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "RetIr\\nid=" << id;
  std::cout << "\"];" << std::endl;
}

void CallExp::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "CallExp\\n";
  std::cout << name;
  std::cout << "\"];" << std::endl;
  for (int i = 0; i < params.size(); i++) {
    params[i]->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << params[i]->id << ";" << std::endl;
  }
}
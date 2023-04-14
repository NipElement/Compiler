#include <ast.hpp>
#include <iostream>

void StartRoot::printTree() {
  std::cout << "digraph G{ " << std::endl;
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "StartRoot\\nid=" << id;
  std::cout << "\"];" << std::endl;
  // print children
  if (comp_unit_ast) {
    comp_unit_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << comp_unit_ast->id << ";" << std::endl;
  }
}

void CompUnitAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  // you want to show in this node  e.g.
  std::cout << "CompUnitAST\\nid=" << id;
  // end
  std::cout << "\"];" << std::endl;
  if (func_def_ast) {
    func_def_ast->printTree();
    // print the relation
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << func_def_ast->id << ";" << std::endl;
  } else if (decl_ast) {
    decl_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << decl_ast->id << ";" << std::endl;
  }

  if (comp_unit_ast) {
    comp_unit_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << comp_unit_ast->id << ";" << std::endl;
  }
}

void VarDeclAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "VarDeclAST\\nid=" << id;
  switch (type) {
    case 0:
      std::cout << "int";
      break;
    case 1:
      std::cout << "void";
  }
  std::cout << "\"];" << std::endl;

  if (var_def_ast) {
    var_def_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << var_def_ast->id << ";" << std::endl;
  }

  if (var_def_list_ast) {
    var_def_list_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << var_def_list_ast->id << ";" << std::endl;
  }
}

void VarDefListAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "VarDeclAST\\nid=" << id;
  std::cout << "\"];" << std::endl;

  if (var_def_ast) {
    var_def_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << var_def_ast->id << ";" << std::endl;
  }

  if (var_def_list_ast) {
    var_def_list_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << var_def_list_ast->id << ";" << std::endl;
  }
}

void VarDefAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "VarDefAST\\nid=" << id;
  std::cout << "\\n" << ident;
  std::cout << "\"];" << std::endl;

  if (exp_list1_ast) {
    exp_list1_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp_list1_ast->id << ";" << std::endl;
  }

  if (initval_ast) {
    initval_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << initval_ast->id << ";" << std::endl;
  }
}

void FuncDefAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "FuncDefAST\\nid=" << id;
  std::cout << "\\n" << ident;
  std::cout << "\"];" << std::endl;

  if (func_fparams_ast) {
    func_fparams_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << func_fparams_ast->id << ";" << std::endl;
  }

  if (block_ast) {
    block_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << block_ast->id << ";" << std::endl;
  }
}

void FuncFParamsAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "FuncFParamsAST\\nid=" << id;

  std::cout << "\"];" << std::endl;

  if (func_fparam_ast) {
    func_fparam_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << func_fparam_ast->id << ";" << std::endl;
  }

  if (func_fparam_list_ast) {
    func_fparam_list_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << func_fparam_list_ast->id << ";" << std::endl;
  }
}

void FuncFParamListAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "FuncFParamListAST\\nid=" << id;

  std::cout << "\"];" << std::endl;

  if (func_fparam_ast) {
    func_fparam_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << func_fparam_ast->id << ";" << std::endl;
  }

  if (func_fparam_list_ast) {
    func_fparam_list_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << func_fparam_list_ast->id << ";" << std::endl;
  }
}

void FuncFParamAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "FuncFParamAST\\nid=" << id;
  std::cout << "\\n" << ident;
  std::cout << "\"];" << std::endl;

  if (exp_list1_ast) {
    exp_list1_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp_list1_ast->id << ";" << std::endl;
  }
}

void BlockAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "BlockAST\\nid=" << id;

  std::cout << "\"];" << std::endl;

  if (block_item_list_ast) {
    block_item_list_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << block_item_list_ast->id << ";" << std::endl;
  }
}

void BlockItemListAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "BlockItemListAST\\nid=" << id;

  std::cout << "\"];" << std::endl;

  if (block_item_ast) {
    block_item_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << block_item_ast->id << ";" << std::endl;
  }

  if (block_item_list_ast) {
    block_item_list_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << block_item_list_ast->id << ";" << std::endl;
  }
}

void BlockItemAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "BlockItemAST\\nid=" << id;

  std::cout << "\"];" << std::endl;

  if (decl_ast) {
    decl_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << decl_ast->id << ";" << std::endl;
  }

  if (stmt_ast) {
    stmt_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << stmt_ast->id << ";" << std::endl;
  }
}

void StmtAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "StmtAST\\nid=" << id;
  std::cout << "\\nstmt_rule=" << stmt_rule;
  std::cout << "\"];" << std::endl;

  if (l_val_ast) {
    l_val_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << l_val_ast->id << ";" << std::endl;
  }

  if (exp_ast) {
    exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp_ast->id << ";" << std::endl;
  }
  if (block_ast) {
    block_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << block_ast->id << ";" << std::endl;
  }
  if (stmt1_ast) {
    stmt1_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << stmt1_ast->id << ";" << std::endl;
  }
  if (stmt2_ast) {
    stmt2_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << stmt2_ast->id << ";" << std::endl;
  }
}
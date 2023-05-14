#include <ast.hpp>
#include <iostream>
#include "irtree.hpp"
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
  std::cout << "}" << std::endl;
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
  std::cout << "\\n" << *ident;
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
  std::cout << "\\n" << *ident;
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
  std::cout << "\\n" << *ident;
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

// jsa
void ExpAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "ExpAST\\nid=" << id;

  if (str) {
    std::cout << "\\nstring = " << str->substr(1, str->length() - 2);
  }

  std::cout << "\"];" << std::endl;
  if (l_or_exp_ast) {
    l_or_exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << l_or_exp_ast->id << ";" << std::endl;
  }
}

void LValAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "LValAST\\nid=" << id;
  std::cout << "\\n" << *ident;
  std::cout << "\"];" << std::endl;

  if (exp_list1_ast) {
    exp_list1_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp_list1_ast->id << ";" << std::endl;
  }
}

void ExpList1AST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "ExpList1AST\\nid=" << id;

  std::cout << "\"];" << std::endl;

  if (exp_ast) {
    exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp_ast->id << ";" << std::endl;
  }

  if (exp_list1_ast) {
    exp_list1_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp_list1_ast->id << ";" << std::endl;
  }
}

void PrimaryExpAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "PrimaryExpAST\\nid=" << id;

  std::cout << "\"];" << std::endl;

  if (exp_ast) {
    exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp_ast->id << ";" << std::endl;
  }

  if (l_val_ast) {
    l_val_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << l_val_ast->id << ";" << std::endl;
  }

  if (number_ast) {
    number_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << number_ast->id << ";" << std::endl;
  }
}

void NumberAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "NumberAST\\nid=" << id;
  std::cout << "\\n" << int_const;
  std::cout << "\"];" << std::endl;
}

void UnaryExpAST::printTree() {
  // tell the rules by the identifier
  if (ident == nullptr) {
    std::cout << "SYNTAX_NODE_" << id << "[label=\"";
    std::cout << "UnaryExpAST\\nid=" << id;

    std::cout << "\"];" << std::endl;

    if (primary_exp_ast) {
      primary_exp_ast->printTree();
      std::cout << "SYNTAX_NODE_" << id << "  ->  "
                << "SYNTAX_NODE_" << primary_exp_ast->id << ";" << std::endl;
    }
    if (unary_op_ast) {
      unary_op_ast->printTree();
      std::cout << "SYNTAX_NODE_" << id << "  ->  "
                << "SYNTAX_NODE_" << unary_op_ast->id << ";" << std::endl;
    }
    if (unary_exp_ast) {
      unary_exp_ast->printTree();
      std::cout << "SYNTAX_NODE_" << id << "  ->  "
                << "SYNTAX_NODE_" << unary_exp_ast->id << ";" << std::endl;
    }
  } else {
    std::cout << "SYNTAX_NODE_" << id << "[label=\"";
    std::cout << "UnaryExpAST\\nid=" << id;
    std::cout << "\\n" << *ident;
    std::cout << "\"];" << std::endl;

    if (func_rparams_ast) {
      func_rparams_ast->printTree();
      std::cout << "SYNTAX_NODE_" << id << "  ->  "
                << "SYNTAX_NODE_" << func_rparams_ast->id << ";" << std::endl;
    }
  }
}

void UnaryOpAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "UnaryOpAST\\nid=" << id;
  std::cout << "\\unary_op_rule=" << unary_op_rule;
  std::cout << "\"];" << std::endl;
}

void FuncRParamsAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "FuncRParamsAST\\nid=" << id;

  std::cout << "\"];" << std::endl;

  if (exp_ast) {
    exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp_ast->id << ";" << std::endl;
  }

  if (exp_list2_ast) {
    exp_list2_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp_list2_ast->id << ";" << std::endl;
  }
}

void ExpList2AST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "ExpList2AST\\nid=" << id;

  std::cout << "\"];" << std::endl;

  if (exp_ast) {
    exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp_ast->id << ";" << std::endl;
  }

  if (exp_list2_ast) {
    exp_list2_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << exp_list2_ast->id << ";" << std::endl;
  }
}

void AddExpAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "AddExpAST\\nid=" << id;
  std::cout << "\\add_exp_rule=" << add_exp_rule;
  std::cout << "\"];" << std::endl;

  if (add_exp_ast) {
    add_exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << add_exp_ast->id << ";" << std::endl;
  }

  if (mul_exp_ast) {
    mul_exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << mul_exp_ast->id << ";" << std::endl;
  }
}

void RelExpAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "RelExpAST\\nid=" << id;
  std::cout << "\\rel_exp_rule=" << rel_exp_rule;
  std::cout << "\"];" << std::endl;

  if (rel_exp_ast) {
    rel_exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << rel_exp_ast->id << ";" << std::endl;
  }

  if (add_exp_ast) {
    add_exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << add_exp_ast->id << ";" << std::endl;
  }
}

void EqExpAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "EqExpAST\\nid=" << id;
  std::cout << "\\eq_rule=" << eq_rule;
  std::cout << "\"];" << std::endl;

  if (eq_exp_ast) {
    eq_exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << eq_exp_ast->id << ";" << std::endl;
  }

  if (rel_exp_ast) {
    rel_exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << rel_exp_ast->id << ";" << std::endl;
  }
}

void LAndExpAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "LAndExpAST\\nid=" << id;
  std::cout << "\\l_and_exp_rule=" << l_and_exp_rule;
  std::cout << "\"];" << std::endl;

  if (l_and_exp_ast) {
    l_and_exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << l_and_exp_ast->id << ";" << std::endl;
  }

  if (eq_exp_ast) {
    eq_exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << eq_exp_ast->id << ";" << std::endl;
  }
}

void LOrExpAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "LOrExpAST\\nid=" << id;
  std::cout << "\\l_or_exp_rule=" << l_or_exp_rule;
  std::cout << "\"];" << std::endl;

  if (l_or_exp_ast) {
    l_or_exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << l_or_exp_ast->id << ";" << std::endl;
  }

  if (l_and_exp_ast) {
    l_and_exp_ast->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << l_and_exp_ast->id << ";" << std::endl;
  }
}

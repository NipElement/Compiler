#include <ast.hpp>
#include <iostream>

void CompUnitAST::printTree() {
  std::cout << "digraph G{ " << std::endl;
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  // you want to show in this node  e.g.
  std::cout << "comp_unit\\nid=" << id;
  // end
  std::cout << "\"];" << std::endl;
  if (func_def != nullptr) {
    func_def->printTree();
    // print the relation
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << func_def->id << ";" << std::endl;
  }
  std::cout << "}" << std::endl;
}
void FuncDefAST::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "func_def\\nid=" << id;
  std::cout << "\\nident=" << ident;
  std::cout << "\"];" << std::endl;

  // print children
  if (func_type != nullptr) {
    func_type->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << func_type->id << ";" << std::endl;
  }
  if (block != nullptr) {
    block->printTree();
    std::cout << "SYNTAX_NODE_" << id << "  ->  "
              << "SYNTAX_NODE_" << block->id << ";" << std::endl;
  }
}
void Block::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "Block\\nid=" << id;
  std::cout << "\"];" << std::endl;

  stmt->printTree();
  std::cout << "SYNTAX_NODE_" << id << "  ->  "
            << "SYNTAX_NODE_" << stmt->id << ";" << std::endl;
}
void FuncType::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "func_type\\nid=" << id << "\\ntype=" << type_;
  std::cout << "\"];" << std::endl;
}
void Stmt::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "stmt\\nid=" << id;
  std::cout << "\"];" << std::endl;

  number->printTree();
  std::cout << "SYNTAX_NODE_" << id << "  ->  "
            << "SYNTAX_NODE_" << number->id << ";" << std::endl;
}
void Number::printTree() {
  std::cout << "SYNTAX_NODE_" << id << "[label=\"";
  std::cout << "number\\nid=" << id << "\\ncontent=" << content;
  std::cout << "\"];" << std::endl;
}
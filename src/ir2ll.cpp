#include <iostream>
#include "irtree.hpp"
int attr_id = 0;
// void RootIr::printLL() {
//   // omit some attributes
//   for (int i = 0; i < funcs.size(); i++) {
//     funcs[i]->printLL();
//   }
// }

// void FuncDefIr::printLL() {
//   std::cout << "define dso_local ";
//   if (type == 0) {
//     std::cout << "i32 ";
//   } else {
//     std::cout << "void ";
//   }
//   std::cout << "@" << name << "(";
//   for (int i = 0; i < param_names.size(); i++) {
//     if (i != 0) {
//       std::cout << ", ";
//     }
//     if (param_types[i] == ParamType(Int)) {
//       std::cout << "i32 ";
//     } else {
//       // ...
//     }

//     std::cout << "%" << i;
//   }
//   std::cout << ")";

//   std::cout << " #" << attr_id << " ";
//   std::cout << "{" << std::endl;

//   block->printLL();

//   std::cout << "}" << std::endl;
// }

// void BlockIr::printLL() {
//   // print
// }

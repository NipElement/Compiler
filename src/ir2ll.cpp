#include <iostream>
#include "irtree.hpp"

using namespace std;

// int attr_id = 0;
// void RootIr::printLL() {
//   // omit some attributes
//   for (int i = 0; i < funcs.size(); i++) {
//     funcs[i]->printLL();
//   }
// }

// void FuncDefIr::printLL() {
//   int main_flag = 0;
//   if (name == "main") {
//     main_flag = 1;
//   }
//   std::cout << "define ";
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
//     cout << "noundef ";
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

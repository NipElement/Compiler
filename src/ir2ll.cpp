#include <assert.h>
#include <algorithm>
#include <iostream>
#include "irtree.hpp"
using namespace std;

int attr_id = 0;

std::string RootIr::replace(std::string str, std::string from, std::string to, int &len) {
  len = str.length();
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();  // Handles case where 'to' is a substring of 'from'

    len -= 1;
  }
  return str;
}

void RootIr::printLL() {
  // omit some attributes
  for (int i = 0; i < BaseIr::const_strings.size(); i++) {
    cout << "@.str";
    if (i != 0) {
      cout << "." << i;
    }
    int len;
    string replace_Escape = replace(BaseIr::const_strings[i], "\\n", "\\0A", len);
    string string_type = "[" + to_string(len + 1) + " x i8]";
    cout << " = private unnamed_addr constant " << string_type;
    cout << " c";

    cout << "\"" << replace_Escape << "\\00"
         << "\", align 1" << endl;
  }
  for (int i = funcs.size() - 1; i >= 0; i--) {
    funcs[i]->printLL();
  }
  cout << "declare i32 @scanf(i8 * noundef, ...) #1" << endl;

  cout << "declare i32 @printf(i8 * noundef, ...) #1" << endl;

  cout << "declare i8* @strcpy(i8*, i8*) #1" << endl;
  cout << "declare i32 @strcmp(i8*, i8*) " << endl;
  cout << "declare i8* @malloc(i64)" << endl;
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
  } else if (var_type == VariableType(CharArray)) {
    std::string array_type = "[" + to_string(size) + " x i8]";
    cout << "  %" << mem_id << " = alloca " << array_type << ", align 1" << endl;
  } else if (var_type == VariableType(CharPtrArray)) {
    std::string array_type = "[" + to_string(size) + " x i8*]";
    cout << "  %" << mem_id << " = alloca " << array_type << ", align 16" << endl;
  }
}

void FuncDefIr::printLL() {
  std::cout << "define ";
  if (ret_type == VariableType(Int)) {
    std::cout << "i32 ";
  } else if (ret_type == VariableType(Void)) {
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
    if ((exp1->res_type == VariableType(Pointer)) && (exp2->exp_type != Const)) {
      /*
      %ptr_temp_reg = sext i32 %exp2->reg_id to i64
      %67 = getelementptr inbounds i32, ptr %64, i64 %66
      */
      cout << "  %" << ptr_temp_reg << " = sext i32 %" << exp2->reg_id << " to i64" << endl;
      cout << "  %" << reg_id << " = getelementptr inbounds i32, ptr %" << exp1->reg_id << ", i64 %" << ptr_temp_reg
           << endl;
    } else {
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
  } else if (op == Mul) {
    if (constFlag == 0) {
      cout << "  %" << reg_id << " = "
           << "mul nsw i32 "
           << "%" << exp1->reg_id << ", %" << exp2->reg_id << endl;
    } else if (exp1->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp1.get());
      // %reg_id = mul nsw i32 theConstExp->value, %exp2->reg_id
      cout << "  %" << reg_id << " = "
           << "mul nsw i32 " << theConstExp->value << ", %" << exp2->reg_id << endl;
    } else if (exp2->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp2.get());
      // %reg_id = mul nsw i32 %exp1->reg_id, theConstExp->value
      cout << "  %" << reg_id << " = "
           << "mul nsw i32 "
           << "%" << exp1->reg_id << ", " << theConstExp->value << endl;
    }
  } else if (op == Divide) {
    if (constFlag == 0) {
      cout << "  %" << reg_id << " = "
           << "sdiv nsw i32 "
           << "%" << exp1->reg_id << ", %" << exp2->reg_id << endl;
    } else if (exp1->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp1.get());
      // %reg_id = sdiv nsw i32 theConstExp->value, %exp2->reg_id
      cout << "  %" << reg_id << " = "
           << "sdiv nsw i32 " << theConstExp->value << ", %" << exp2->reg_id << endl;
    } else if (exp2->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp2.get());
      // %reg_id = sdiv nsw i32 %exp1->reg_id, theConstExp->value
      cout << "  %" << reg_id << " = "
           << "sdiv nsw i32 "
           << "%" << exp1->reg_id << ", " << theConstExp->value << endl;
    }
  }
}

void NorBoolBinopExp::printLL() {
  if (exp1->exp_type != Const) {
    exp1->printLL();
  }
  if (exp2->exp_type != Const) {
    exp2->printLL();
  }
  int constFlag = 0;
  constFlag = (exp1->exp_type == Const) + (exp2->exp_type == Const);

  if (op == Ge) {
    if (constFlag == 0) {
      // %bool_result_reg = icmp sge i32 %exp1->reg_id, %exp2->reg_id
      // %reg_id = zext i1 %10 to i32
      cout << "  %" << bool_result_reg << " = icmp sge i32 %" << exp1->reg_id << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
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
      cout << "  %" << bool_result_reg << " = icmp sge i32 %" << exp1->reg_id << ", " << theConstExp->value << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    }
  } else if (op == Le) {
    if (constFlag == 0) {
      // %bool_result_reg = icmp sle i32 %exp1->reg_id, %exp2->reg_id
      // %reg_id = zext i1 %10 to i32
      cout << "  %" << bool_result_reg << " = icmp sle i32 %" << exp1->reg_id << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    } else if (exp1->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp1.get());
      // %bool_result_reg = icmp sle i32 const, %exp2->reg_id
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp sle i32 " << theConstExp->value << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    } else if (exp2->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp2.get());
      // %bool_result_reg = icmp sle i32 %exp2->reg_id, const
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp sle i32 %" << exp1->reg_id << ", " << theConstExp->value << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    }
  } else if (op == Gt) {
    if (constFlag == 0) {
      // %bool_result_reg = icmp sgt i32 %exp1->reg_id, %exp2->reg_id
      // %reg_id = zext i1 %10 to i32
      cout << "  %" << bool_result_reg << " = icmp sgt i32 %" << exp1->reg_id << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    } else if (exp1->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp1.get());
      // %bool_result_reg = icmp sgt i32 const, %exp2->reg_id
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp sgt i32 " << theConstExp->value << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    } else if (exp2->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp2.get());
      // %bool_result_reg = icmp sgt i32 %exp2->reg_id, const
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp sgt i32 %" << exp1->reg_id << ", " << theConstExp->value << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    }
  } else if (op == Lt) {
    if (constFlag == 0) {
      // %bool_result_reg = icmp slt i32 %exp1->reg_id, %exp2->reg_id
      // %reg_id = zext i1 %10 to i32
      cout << "  %" << bool_result_reg << " = icmp slt i32 %" << exp1->reg_id << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    } else if (exp1->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp1.get());
      // %bool_result_reg = icmp slt i32 const, %exp2->reg_id
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp slt i32 " << theConstExp->value << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    } else if (exp2->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp2.get());
      // %bool_result_reg = icmp slt i32 %exp2->reg_id, const
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp slt i32 %" << exp1->reg_id << ", " << theConstExp->value << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    }
  } else if (op == Eq) {
    if (constFlag == 0) {
      // %bool_result_reg = icmp eq i32 %exp1->reg_id, %exp2->reg_id
      // %reg_id = zext i1 %10 to i32
      cout << "  %" << bool_result_reg << " = icmp eq i32 %" << exp1->reg_id << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    } else if (exp1->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp1.get());
      // %bool_result_reg = icmp eq i32 const, %exp2->reg_id
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp eq i32 " << theConstExp->value << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    } else if (exp2->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp2.get());
      // %bool_result_reg = icmp eq i32 %exp2->reg_id, const
      // %reg_id = zext i1 %bool_result_reg to i32
      // jsc 
      if (exp1->res_type == VariableType(Char)) {
        cout << "  %" << bool_result_reg << " = icmp eq i8 %" << exp1->reg_id << ", " << theConstExp->value << endl;
        cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
      } else {
        cout << "  %" << bool_result_reg << " = icmp eq i32 %" << exp1->reg_id << ", " << theConstExp->value << endl;
        cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
      }
    }
  } else if (op == Ne) {
    if (constFlag == 0) {
      // %bool_result_reg = icmp ne i32 %exp1->reg_id, %exp2->reg_id
      // %reg_id = zext i1 %10 to i32
      cout << "  %" << bool_result_reg << " = icmp ne i32 %" << exp1->reg_id << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    } else if (exp1->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp1.get());
      // %bool_result_reg = icmp ne i32 const, %exp2->reg_id
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp ne i32 " << theConstExp->value << ", %" << exp2->reg_id << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    } else if (exp2->exp_type == Const) {
      ConstExp *theConstExp = dynamic_cast<ConstExp *>(exp2.get());
      // %bool_result_reg = icmp ne i32 %exp2->reg_id, const
      // %reg_id = zext i1 %bool_result_reg to i32
      cout << "  %" << bool_result_reg << " = icmp ne i32 %" << exp1->reg_id << ", " << theConstExp->value << endl;
      cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
    }
  }
}

// here for the And Or
void AndOrBinopExp::printLL() {
  // we assume there is no const exp

  exp1->printLL();
  // %bool_res1 = icmp ne i32 %exp1->reg_id, 0
  cout << "  %" << bool_res1 << " = icmp ne i32 %" << exp1->reg_id << ", 0" << endl;
  // br i1 %8, label %9, label %14
  cout << "  br i1 %" << bool_res1 << ", label %" << label1 << ", label %" << label2 << endl;
  cout << label1 << ":" << endl;
  exp2->printLL();
  // %bool_res2 = icmp ne i32 %exp2->reg_id, 0
  cout << "  %" << bool_res2 << " = icmp ne i32 %" << exp2->reg_id << ", 0" << endl;
  // br label %label2
  cout << "  br label %" << label2 << endl;
  cout << label2 << ":" << endl;

  if (op == And) {
    // %15 = phi i1 [ false, %1 ], [ %13, %9 ]
    cout << "  %" << bool_result_reg << " = phi i1 [ false, %" << last_block_label << " ], [ %" << bool_res2 << ", %"
         << label1 << " ]" << endl;
  } else if (op == Or) {
    // %23 = phi i1 [ true, %14 ], [ %21, %19 ]
    cout << "  %" << bool_result_reg << " = phi i1 [ true, %" << last_block_label << " ], [ %" << bool_res2 << ", %"
         << label1 << " ]" << endl;
  }
  // %16 = zext i1 %15 to i32
  cout << "  %" << reg_id << " = zext i1 %" << bool_result_reg << " to i32" << endl;
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

  } else if (mem_type == VariableType(Array) || mem_type == VariableType(CharArray) ||
             mem_type == VariableType(CharPtrArray)) {
    // array element as left value: like a[exp]=..
    if (exp->exp_type != ExpType(Const)) {  // a[exp]
      exp->printLL();
      // sext i32 %12 to i64
      cout << "  %" << signext_id << " = sext i32 %" << exp->reg_id << " to i64" << endl;
      // %10 = getelementptr inbounds [10 x i32], [10 x i32]* %3, i64 0, i64 %9
      std::string array_type;
      if (mem_type == VariableType(Array)) {
        array_type = "[" + to_string(size) + " x i32]";
      } else if (mem_type == VariableType(CharArray)) {
        array_type = "[" + to_string(size) + " x i8]";
      } else {
        array_type = "[" + to_string(size) + " x i8*]";
      }
      cout << "  %" << reg_id << " = getelementptr inbounds ";
      cout << array_type << ", " << array_type << "*"
           << " %" << array_reg_id << ", i64 0, i64 %" << signext_id << std::endl;
    } else {  // const exp a[1]
      std::string array_type;
      if (mem_type == VariableType(Array)) {
        array_type = "[" + to_string(size) + " x i32]";
      } else if (mem_type == VariableType(CharArray)) {
        array_type = "[" + to_string(size) + " x i8]";
      } else {
        array_type = "[" + to_string(size) + " x i8*]";
      }
      cout << "  %" << reg_id << " = getelementptr inbounds ";
      cout << array_type << ", " << array_type << "*"
           << " %" << array_reg_id << ", i64 0, i64 " << dynamic_cast<ConstExp *>(exp.get())->value << std::endl;
    }
  } else if (mem_type == VariableType(Pointer) || mem_type == VariableType(CharPointer)) {
    if (exp->exp_type != ExpType(Const)) {  // a[exp]
      std::string pointer_type = "i32*";
      std::string pointed_type = "i32";
      if (mem_type == VariableType(CharPointer)) {
        pointer_type = "i8*";
        pointed_type = "i8";
      }
      // load pointer value from memory
      cout << "  %" << pointer_value_reg_id << " = load " << pointer_type << ", " << pointer_type << "*"
           << " %" << array_reg_id << ", align 8" << endl;
      // pointer with [] as left value, compute the exp
      exp->printLL();
      //%9 = sext i32 %8 to i64
      cout << "  %" << signext_id << " = sext i32 %" << exp->reg_id << " to i64" << endl;
      // %8 = getelementptr inbounds i32, i32* %7, i64 1

      cout << "  %" << reg_id << " = getelementptr inbounds " << pointed_type << ", " << pointer_type << "%"
           << pointer_value_reg_id << ", "
           << "i64 %" << signext_id << endl;
    } else {  // const exp a[1]
      std::string pointer_type = "i32*";
      std::string pointed_type = "i32";
      if (mem_type == VariableType(CharPointer)) {
        pointer_type = "i8*";
        pointed_type = "i8";
      }
      // load pointer value from memory
      cout << "  %" << pointer_value_reg_id << " = load " << pointer_type << ", " << pointer_type << "*"
           << " %" << array_reg_id << ", align 8" << endl;

      cout << "  %" << reg_id << " = getelementptr inbounds " << pointed_type << ", " << pointer_type << "%"
           << pointer_value_reg_id << ", "
           << "i64 " << dynamic_cast<ConstExp *>(exp.get())->value << endl;
    }
  }
}

void TempExp::printLL() {
  // calculate the location
  mem->printLL();

  auto mem_exp = dynamic_cast<MemExp *>(mem.get());
  if (res_type == VariableType(Int) || res_type == VariableType(Char) ||
      (res_type == VariableType(CharPointer) &&
       mem_exp->mem_type == VariableType(CharPtrArray))) {  // for simple int or int or char a[i] or char* a[i]
    // %10 = load i32, i32* %6, align 4
    std::string res_type_str = "i32";
    if (res_type == VariableType(Char)) {
      res_type_str = "i8";
    } else if (res_type == VariableType(CharPointer)) {
      res_type_str = "i8*";
    }
    cout << "  %" << reg_id << " = load " << res_type_str << ", " << res_type_str << "* %" << mem->reg_id << ", align 4"
         << endl;
  } else if (mem_exp->mem_type == VariableType(Array) || mem_exp->mem_type == VariableType(CharArray) ||
             mem_exp->mem_type == VariableType(CharPtrArray)) {  // array as function parameter passing
    //%10 = getelementptr inbounds [10 x i32], [10 x i32]* %7, i64 0, i64 0
    string array_type = "[" + to_string(mem_exp->size) + " x i32]";
    if (mem_exp->mem_type == VariableType(CharArray)) {
      array_type = "[" + to_string(mem_exp->size) + " x i8]";
    } else if (mem_exp->mem_type == VariableType(CharPtrArray)) {
      array_type = "[" + to_string(mem_exp->size) + " x i8*]";
      // we suppose char* a[] will not be passing to function, will this work ?
    }
    cout << "  %" << reg_id << " = "
         << "getelementptr inbounds ";
    cout << array_type << ", " << array_type << "* "
         << "%" << mem->reg_id << ", i64 0, i64 0" << endl;
  } else if (mem_exp->mem_type == VariableType(Pointer) ||
             mem_exp->mem_type == VariableType(CharPointer)) {  // pointer as function parameter passing
    // %8 = load i32*, i32** %3, align 8
    std::string res_type_str = "i32*";
    if (mem_exp->mem_type == VariableType(CharPointer)) {
      res_type_str = "i8*";
    }
    cout << "  %" << reg_id << " = "
         << "load i32*, i32** %" << mem->reg_id << ", align 8" << endl;
  }
}

void ConstExp::printLL() {
  // store i32 value, i32* %reg_id, align 4
  // cout << "  store i32 " << value << ", i32 * %" << reg_id << ", align 4" << endl;
}

void CallExp::printLL() {
  // todo: check the function is lib function or not
  if (name == "printf") {
    // prepare parameter
    for (int i = 0; i < params.size(); i++) {
      params[i]->printLL();
    }
    // get the string
    auto string = dynamic_cast<ConstExp *>(params[0].get())->str;
    auto it = std::find(BaseIr::const_strings.begin(), BaseIr::const_strings.end(), string);
    // get the string index
    int index = std::distance(BaseIr::const_strings.begin(), it);

    // %7 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([8 x i8], [8 x i8]* @.str.1, i64 0, i64
    // 0), i32 noundef %6)
    std::string const_str_type = "[" + to_string(string.length() + 1) + " x i8]";
    std::cout << "  %" << reg_id << " = call i32 (i8*, ...) @printf"
              << "(i8* noundef getelementptr inbounds (" << const_str_type << ", " << const_str_type << "* @.str";
    if (index != 0) {
      std::cout << "." << index;
    }

    std::cout << ",i64 0, i64 0)";

    for (int i = 1; i < params.size(); i++) {
      cout << ", ";
      // we always assume that the paras in printf is Temp
      // here we implement both
      if (params[i]->exp_type == ExpType(Temp)) {
        auto temp = dynamic_cast<TempExp *>(params[i].get());
        if (temp->res_type == VariableType(CharPointer)) {  // passing to printf like array, which is not permitted
          cout << "i8* "
               << "%" << params[i]->reg_id;
        } else if (temp->res_type == VariableType(Int)) {
          cout << "i32 "
               << "%" << params[i]->reg_id;
        } else {
          assert(0);
        }
      } else if (params[i]->exp_type == ExpType(Mem)) {  // like printf("%d",&a) or printf("%d",&a[1])
        cout << "i32* "
             << "%" << params[i]->reg_id;
      } else {  // any other exp like binop,
        cout << "i32 "
             << "%" << params[i]->reg_id;
      }
    }
    cout << ")" << endl;
    return;
  } else if (name == "scanf") {
    // prepare parameter
    for (int i = 0; i < params.size(); i++) {
      params[i]->printLL();
    }
    // get the string
    auto string = dynamic_cast<ConstExp *>(params[0].get())->str;
    auto it = std::find(BaseIr::const_strings.begin(), BaseIr::const_strings.end(), string);
    // get the string index
    int index = std::distance(BaseIr::const_strings.begin(), it);

    std::string const_str_type = "[" + to_string(string.length() + 1) + " x i8]";

    std::cout << "  %" << reg_id << " = call i32 (i8*, ...) @scanf"
              << "(i8* noundef getelementptr inbounds (" << const_str_type << ", " << const_str_type << "* @.str";

    if (index != 0) {
      std::cout << "." << index;
    }
    std::cout << ",i64 0, i64 0)";

    // we always assume scanf has only & lval, so it is MemExp
    for (int i = 1; i < params.size(); i++) {
      cout << ", ";
      if (params[i]->exp_type == ExpType(Mem)) {
        auto mem = dynamic_cast<MemExp *>(params[i].get());
        // like &a or &a[1]
        cout << "i32* noundef "
             << "%" << mem->reg_id;
      } else if (params[i]->exp_type == ExpType(Temp)) {
        std::string temp_type = "i8*";
        auto temp = dynamic_cast<TempExp *>(params[i].get());
        // like &a or &a[1]
        cout << temp_type << " noundef "
             << "%" << temp->reg_id;
      }
    }
    cout << ")" << endl;
    return;
  }

  // first compute all the expression as parameter
  for (int i = 0; i < params.size(); i++) {
    params[i]->printLL();
  }

  // if the return type is void, no reg_id is used
  if (ret_type == VariableType(Void)) {
    // call void @f1(i32 %7, i32 %8)
    cout << "  call void @" << name;
    cout << "(";
  } else if (ret_type == VariableType(Int)) {
    // % 4 = call i32(i8 *, ...) @printf(i8 * getelementptr inbounds([9 x i8], [9 x i8] * @.str .1, i64 0, i64 0), i32
    // %3)
    cout << "  %" << reg_id << " = "
         << "call i32 @" << name;
    cout << "(";
  } else if (ret_type == VariableType(CharPointer)) {  // for malloc
    cout << "  %" << reg_id << " = "
         << "call i8* @" << name;
    cout << "(";
  }

  // parameter
  for (int i = 0; i < params.size(); i++) {
    if (name == "malloc") {  // special for malloc
      cout << "i64 " << dynamic_cast<ConstExp *>(params[i].get())->value;
      break;
    }

    if (i != 0) {
      cout << ", ";
    }
    if (params[i]->exp_type == ExpType(Const)) {
      auto const_exp = dynamic_cast<ConstExp *>(params[i].get());
      if (const_exp->const_type == ConstType(CInt)) {
        cout << "i32 " << dynamic_cast<ConstExp *>(params[i].get())->value;
      } else {
        // get the string
        auto string = dynamic_cast<ConstExp *>(params[i].get())->str;
        auto it = std::find(BaseIr::const_strings.begin(), BaseIr::const_strings.end(), string);
        // get the string index
        int index = std::distance(BaseIr::const_strings.begin(), it);

        // %7 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([8 x i8], [8 x i8]* @.str.1, i64 0, i64
        // 0), i32 noundef %6)
        std::string const_str_type = "[" + to_string(string.length() + 1) + " x i8]";

        std::cout << "i8* noundef getelementptr inbounds (" << const_str_type << ", " << const_str_type << "* @.str";
        if (index != 0) {
          std::cout << "." << index;
        }
        std::cout << ")";
      }
    } else if (params[i]->exp_type == ExpType(Temp)) {
      if (params[i]->res_type == VariableType(Int)) {  // passing like a (int)
        cout << "i32 "
             << "%" << params[i]->reg_id;
      } else if (params[i]->res_type == VariableType(Pointer) ||
                 params[i]->res_type == VariableType(Array)) {  // passing like a[],
        cout << "i32* "
             << "%" << params[i]->reg_id;
      } else if (params[i]->res_type == VariableType(CharPointer)) {
        cout << "i8* "
             << "%" << params[i]->reg_id;
      }
    } else if (params[i]->exp_type == ExpType(Mem)) {  // passing like &a
      assert(0);
    } else if (params[i]->exp_type == ExpType(Binop)) {  // like binop and other exp
      if (params[i]->res_type == VariableType(Pointer)) {
        cout << "i32* "
             << "%" << params[i]->reg_id;
      } else {
        cout << "i32 "
             << "%" << params[i]->reg_id;
      }
    }
  }

  cout << ")" << endl;
}

void MoveIr::printLL() {
  // Value
  if (exp2->exp_type != ExpType(Const)) {
    exp2->printLL();
  }
  // Mem
  exp1->printLL();
  if (exp2->exp_type == ExpType(Const)) {  // a or a[i]
    cout << "  store i32 " << dynamic_cast<ConstExp *>(exp2.get())->value << ", i32* %" << exp1->reg_id << ", align 4"
         << endl;
    return;
  } else if (exp2->exp_type == ExpType(Call)) {
    auto exp2_call = dynamic_cast<CallExp *>(exp2.get());
    if (exp2_call->ret_type == VariableType(CharPointer)) {  // for malloc
      cout << "  store i8* %" << exp2->reg_id << ", i8** %" << exp1->reg_id << ", align 4" << endl;
    } else {
      cout << "  store i32 %" << exp2->reg_id << ", i32* %" << exp1->reg_id << ", align 4" << endl;
    }
  } else {
    cout << "  store i32 %" << exp2->reg_id << ", i32* %" << exp1->reg_id << ", align 4" << endl;
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
  // cout << t << ":" << endl;
  // t_block->printLL();
  // cout << "  br label %" << done << endl;
  // f:
  // cout << endl;
  // cout << f << ":" << endl;
  // f_block->printLL();
  // cout << "  br label %" << done << endl;
  // done:
  // cout << endl;
  // cout << done << ":" << endl;
}

void JumpIr::printLL() { cout << "  br label %" << label << endl; }

void LabelIr::printLL() { cout << label << ":" << endl; }

void RetIr::printLL() {
  // currently only support ret void
  if (ret_value == nullptr) {
    cout << "  ret void" << endl;
  } else {
    if (ret_value->exp_type == ExpType(Const)) {
      cout << "  ret i32 " << dynamic_cast<ConstExp *>(ret_value.get())->value << endl;
    } else {
      ret_value->printLL();

      // we always assume return a temp: that is no expression like "return &a"..
      cout << "  ret i32 %" << ret_value->reg_id << endl;
    }
  }
}
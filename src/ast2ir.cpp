#include <assert.h>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>
#include "ast.hpp"
#include "irtree.hpp"

std::unordered_map<std::string, int> array_size_table;
std::unordered_map<std::string, int> table;
std::unordered_map<std::string, VariableType> variable_type_table;

std::unordered_map<std::string, VariableType> func_rettype_table;

std::vector<std::string> BaseIr::const_strings;

// id is for printTree
// int ir_id = 0;
// reg is the $..
std::vector<int> all_label;
int reg = 0;
BaseIr *StartRoot::buildIrTree() {
  auto ir_root = new RootIr();
  // ir_root->id = ir_id++;
  auto p = dynamic_cast<CompUnitAST *>(comp_unit_ast.get());
  func_rettype_table.insert(std::pair<std::string, VariableType>(std::string("printf"), VariableType(Int)));
  func_rettype_table.insert(std::pair<std::string, VariableType>(std::string("scanf"), VariableType(Int)));
  while (p) {
    // here I assume p has only funcdef, be careful !
    ir_root->funcs.push_back(std::unique_ptr<BaseIr>((p->func_def_ast.get())->buildIrTree()));
    p = dynamic_cast<CompUnitAST *>(p->comp_unit_ast.get());
  }
  return ir_root;
}

BaseIr *FuncDefAST::buildIrTree() {
  // refresh regs and hash table
  reg = 0;
  table.clear();
  array_size_table.clear();
  variable_type_table.clear();
  all_label.clear();

  auto func_def = new FuncDefIr();
  // func_def->type = IrType(FuncDef);
  // func_def->id = ir_id++;
  if (type == 0) {
    // int
    func_def->ret_type = VariableType(Int);
  } else {
    // void
    func_def->ret_type = VariableType(Void);
  }

  // update func_rettype_table
  func_rettype_table.insert(std::pair<std::string, VariableType>(*ident, func_def->ret_type));
  func_def->name = *ident;

  auto p = dynamic_cast<FuncFParamsAST *>(func_fparams_ast.get());

  // generate parameter list
  if (p) {
    // it must have a child ?
    assert(p->func_fparam_ast != nullptr);
    auto first_para = dynamic_cast<FuncFParamAST *>(p->func_fparam_ast.get());

    if (first_para->type == 0) {
      func_def->param_types.push_back(VariableType(Int));
    } else {
      func_def->param_types.push_back(VariableType(Pointer));
    }

    func_def->param_names.push_back(*(first_para->ident));
    reg++;
    // push parameter in lists
    if (p->func_fparam_list_ast) {
      auto it = dynamic_cast<FuncFParamListAST *>(p->func_fparam_list_ast.get());

      while (it) {
        auto para_leaf = dynamic_cast<FuncFParamAST *>(it->func_fparam_ast.get());

        func_def->param_types.push_back(VariableType(para_leaf->type));
        func_def->param_names.push_back(*(para_leaf->ident));
        reg++;

        it = dynamic_cast<FuncFParamListAST *>(it->func_fparam_list_ast.get());
      }
    }
  }

  reg++;
  for (int i = 0; i < func_def->param_names.size(); i++) {
    table.insert(std::pair<std::string, int>(func_def->param_names[i], i + 1 + func_def->param_names.size()));

    // record the variable type
    variable_type_table.insert(
        std::pair<std::string, VariableType>(func_def->param_names[i], func_def->param_types[i]));
  }

  reg = 2 * func_def->param_names.size() + 1;

  // jsa
  // first label
  all_label.push_back(func_def->param_names.size());

  // generate block
  func_def->block = std::unique_ptr<BaseIr>(dynamic_cast<BlockAST *>(block_ast.get())->buildIrTree());

  dynamic_cast<BlockIr *>(func_def->block.get())->param_num = func_def->param_names.size();

  // if the function returns void, add a return stmt to block
  if (type == 1) {  // return void
    auto ret_stmt = new RetIr();
    // ret_stmt->id = ir_id++;
    // ret_stmt->type = IrType(Ret);
    ret_stmt->ret_value = nullptr;

    dynamic_cast<BlockIr *>(func_def->block.get())->stmts.push_back(std::unique_ptr<BaseIr>(ret_stmt));
  }
  return func_def;
}

BaseIr *BlockAST::buildIrTree() {
  auto block = new BlockIr();
  // block->type = IrType(Block);
  // block->id = ir_id++;

  auto p = dynamic_cast<BlockItemListAST *>(block_item_list_ast.get());
  // for every block item, add it to block->stmts
  while (p) {
    auto block_item_leaf = dynamic_cast<BlockItemAST *>(p->block_item_ast.get());

    auto ret_ir_nodes = block_item_leaf->buildIrNodes();
    // block->stmts.push_back(std::unique_ptr<BaseIr>(block_item_leaf->buildIrTree()));
    for (auto el : ret_ir_nodes) {
      block->stmts.push_back(std::unique_ptr<BaseIr>(el));
    }

    p = dynamic_cast<BlockItemListAST *>(p->block_item_list_ast.get());
  }

  return block;
}

BaseIr *BlockItemAST::buildIrTree() {
  if (decl_ast) {
    // here only consider VarDecl for convenience, and no vardeflist

    auto var_decl = dynamic_cast<VarDeclAST *>(decl_ast.get());
    // suppose no vardef list and check
    assert(var_decl->var_def_list_ast == nullptr);

    auto var_def = dynamic_cast<VarDefAST *>(var_decl->var_def_ast.get());

    // suppose no initial value and check
    assert(var_def->initval_ast == nullptr);

    if (var_def->exp_list1_ast == nullptr) {  // simple varaible: int
      auto decl = new VarDeclIr();
      decl->var_type = VariableType(Int);
      // decl->type = IrType(VarDecl);
      // decl->id = ir_id++;
      decl->mem_id = reg++;
      table.insert(std::pair<std::string, int>(*var_def->ident, decl->mem_id));
      variable_type_table.insert(std::pair<std::string, VariableType>(*var_def->ident, VariableType(Int)));

      return decl;
    } else {  // array
      auto decl = new VarDeclIr();
      decl->var_type = VariableType(Array);

      // decl->type = IrType(VarDecl);
      // decl->id = ir_id++;
      decl->mem_id = reg++;
      table.insert(std::pair<std::string, int>(*var_def->ident, decl->mem_id));
      variable_type_table.insert(std::pair<std::string, VariableType>(*var_def->ident, VariableType(Array)));

      // suppose the expression could only be a const and check
      auto exp_list1 = dynamic_cast<ExpList1AST *>(var_def->exp_list1_ast.get());

      // suppose there is no exp_list and check
      assert(exp_list1->exp_list1_ast == nullptr);

      auto exp = dynamic_cast<ExpAST *>(exp_list1->exp_ast.get());

      int size = exp->getExpNum();
      decl->size = size;

      array_size_table.insert(std::pair<std::string, int>(*var_def->ident, size));

      return decl;
    }

  } else if (stmt_ast) {
    auto stmt = dynamic_cast<StmtAST *>(stmt_ast.get());
    return stmt->buildIrTree();
  }
}

std::vector<BaseIr *> BlockItemAST::buildIrNodes() {
  if (decl_ast) {
    // here only consider VarDecl for convenience, and no vardeflist

    auto var_decl = dynamic_cast<VarDeclAST *>(decl_ast.get());
    // suppose no vardef list and check
    assert(var_decl->var_def_list_ast == nullptr);

    auto var_def = dynamic_cast<VarDefAST *>(var_decl->var_def_ast.get());

    // suppose no initial value and check
    assert(var_def->initval_ast == nullptr);

    if (var_def->exp_list1_ast == nullptr) {  // simple varaible: int
      auto decl = new VarDeclIr();
      decl->var_type = VariableType(Int);
      // decl->type = IrType(VarDecl);
      // decl->id = ir_id++;
      decl->mem_id = reg++;
      table.insert(std::pair<std::string, int>(*var_def->ident, decl->mem_id));
      variable_type_table.insert(std::pair<std::string, VariableType>(*var_def->ident, VariableType(Int)));

      return std::vector<BaseIr *>{decl};
    } else {  // array
      auto decl = new VarDeclIr();
      decl->var_type = VariableType(Array);

      // decl->type = IrType(VarDecl);
      // decl->id = ir_id++;
      decl->mem_id = reg++;
      table.insert(std::pair<std::string, int>(*var_def->ident, decl->mem_id));
      variable_type_table.insert(std::pair<std::string, VariableType>(*var_def->ident, VariableType(Array)));

      // suppose the expression could only be a const and check
      auto exp_list1 = dynamic_cast<ExpList1AST *>(var_def->exp_list1_ast.get());

      // suppose there is no exp_list and check
      assert(exp_list1->exp_list1_ast == nullptr);

      auto exp = dynamic_cast<ExpAST *>(exp_list1->exp_ast.get());

      int size = exp->getExpNum();
      decl->size = size;

      array_size_table.insert(std::pair<std::string, int>(*var_def->ident, size));

      return std::vector<BaseIr *>{decl};
    }

  } else if (stmt_ast) {
    return stmt_ast->buildIrNodes();
  }
}

BaseIr *StmtAST::buildIrTree() {
  if (stmt_rule == 0) {  // stmt_rule = 0 : LVal '=' Exp ';'
    auto move = new MoveIr();
    // move->id = ir_id++;
    // move->type = IrType(Move);

    // in fact this is a move ir
    auto l_val = dynamic_cast<LValAST *>(l_val_ast.get());
    auto exp = dynamic_cast<ExpAST *>(exp_ast.get());
    move->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(exp->buildIrTree()));
    move->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(l_val->buildIrTree()));
    return move;
  } else if (stmt_rule == 2) {  // EXP: function
    return exp_ast->buildIrTree();
  } else if (stmt_rule == 3) {  // stmt_rule = 3 : Block
    return block_ast->buildIrTree();
  } else if (stmt_rule == 5) {  // stmt_rule = 5 :  IF '(' Exp ')' Stmt ELSE Stmt
    auto cjump = new CjumpIr();
    // cjump->id = ir_id++;
    // cjump->type = IrType(Cjump);

    cjump->exp = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(exp_ast->buildIrTree()));

    cjump->condition_reg = reg++;
    cjump->t = reg++;
    all_label.push_back(cjump->t);
    cjump->t_block = std::unique_ptr<BaseIr>(stmt1_ast->buildIrTree());
    cjump->f = reg++;
    all_label.push_back(cjump->f);
    cjump->f_block = std::unique_ptr<BaseIr>(stmt2_ast->buildIrTree());
    cjump->done = reg++;
    all_label.push_back(cjump->done);
    return cjump;
  } else if (stmt_rule == 6) {  // stmt_rule = 6 :  WHILE '(' Exp ')' Stmt
  }
  return nullptr;
}

std::vector<BaseIr *> StmtAST::buildIrNodes() {
  // test
  if (stmt_rule == 0) {  // stmt_rule = 0 : LVal '=' Exp ';'
    auto move = new MoveIr();
    // move->id = ir_id++;
    // move->type = IrType(Move);

    // in fact this is a move ir
    auto l_val = dynamic_cast<LValAST *>(l_val_ast.get());
    auto exp = dynamic_cast<ExpAST *>(exp_ast.get());
    move->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(exp->buildIrTree()));
    move->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(l_val->buildIrTree()));
    return std::vector<BaseIr *>{move};
  } else if (stmt_rule == 2) {  // stmt_rule == 2:Exp
    return std::vector<BaseIr *>{exp_ast->buildIrTree()};
  } else if (stmt_rule == 3) {  // stmt_rule = 3 : Block
    return std::vector<BaseIr *>{block_ast->buildIrTree()};
  } else if (stmt_rule == 5) {  // stmt_ruir_vector;le = 5 :  IF '(' Exp ')' Stmt ELSE Stmt
    std::vector<BaseIr *> ret_ir_vector;
    auto cjump = new CjumpIr();
    ret_ir_vector.push_back(cjump);

    // cjump->id = ir_id++;
    // cjump->type = IrType(Cjump);

    cjump->exp = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(exp_ast->buildIrTree()));

    cjump->condition_reg = reg++;

    cjump->t = reg++;
    all_label.push_back(cjump->t);
    auto t_label = new LabelIr();
    t_label->label = cjump->t;

    auto t_block = stmt1_ast->buildIrTree();
    cjump->t_block = nullptr;

    cjump->f = reg++;
    all_label.push_back(cjump->f);
    auto f_label = new LabelIr();
    f_label->label = cjump->f;
    auto f_block = stmt2_ast->buildIrTree();
    cjump->f_block = nullptr;

    auto t_block_jump_to_done = new JumpIr();
    auto f_block_jump_to_done = new JumpIr();
    // t_block_jump_to_done->type = IrType(Jump);
    // f_block_jump_to_done->type = IrType(Jump);
    // t_block_jump_to_done->id = ir_id++;
    // f_block_jump_to_done->id = ir_id++;

    cjump->done = reg++;
    all_label.push_back(cjump->done);

    t_block_jump_to_done->label = cjump->done;
    f_block_jump_to_done->label = cjump->done;

    ret_ir_vector.push_back(t_label);
    ret_ir_vector.push_back(t_block);
    ret_ir_vector.push_back(t_block_jump_to_done);
    ret_ir_vector.push_back(f_label);
    ret_ir_vector.push_back(f_block);
    ret_ir_vector.push_back(f_block_jump_to_done);

    auto label_done = new LabelIr();
    label_done->label = cjump->done;
    ret_ir_vector.push_back(label_done);

    return ret_ir_vector;
  } else if (stmt_rule == 6) {  // stmt_rule = 6 :  WHILE '(' Exp ')' Stmt
    std::vector<BaseIr *> ret_ir_vector;

    auto jump1 = new JumpIr();
    ret_ir_vector.push_back(jump1);

    auto check_label = new LabelIr();
    ret_ir_vector.push_back(check_label);

    check_label->label = reg++;
    jump1->label = check_label->label;

    auto cjump = new CjumpIr();
    ret_ir_vector.push_back(cjump);

    cjump->exp = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(exp_ast->buildIrTree()));

    cjump->condition_reg = reg++;

    cjump->t = reg++;
    all_label.push_back(cjump->t);
    auto t_label = new LabelIr();
    ret_ir_vector.push_back(t_label);

    t_label->label = cjump->t;

    auto t_block = stmt1_ast->buildIrTree();
    ret_ir_vector.push_back(t_block);

    auto jump_check = new JumpIr();
    ret_ir_vector.push_back(jump_check);
    jump_check->label = check_label->label;

    auto done_label = new LabelIr();
    ret_ir_vector.push_back(done_label);
    done_label->label = reg++;

    // cjump f_label is done_label
    cjump->f = done_label->label;
    all_label.push_back(cjump->f);
    return ret_ir_vector;
  } else if (stmt_rule == 10) {  // return exp
    std::vector<BaseIr *> ret_ir_vector;
    auto ret = new RetIr();

    ret->ret_value = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(exp_ast->buildIrTree()));

    ret_ir_vector.push_back(ret);

    return ret_ir_vector;
  }

  return std::vector<BaseIr *>();
}

BaseIr *LValAST::buildIrTree() {
  auto mem = new MemExp();
  // mem->id = ir_id++;
  // mem->type = IrType(Exp);
  mem->exp_type = ExpType(Mem);

  // get type info
  mem->mem_type = variable_type_table.find(*ident)->second;
  if (mem->mem_type == VariableType(Int)) {  // simple variable int
    mem->signext_id = -1;
    mem->exp = nullptr;
    // the address of this lval
    mem->reg_id = table.find(*ident)->second;

  } else if (exp_list1_ast != nullptr) {  // array or pointer with exp(as left value)
    if (mem->mem_type == VariableType(Pointer)) {
      mem->pointer_value_reg_id = reg++;
    }

    // a[exp]..
    auto exp_list1 = dynamic_cast<ExpList1AST *>(exp_list1_ast.get());

    // suppose no exp combined by ','
    assert(exp_list1->exp_list1_ast == nullptr);

    auto exp = dynamic_cast<ExpAST *>(exp_list1->exp_ast.get());

    mem->exp = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(exp->buildIrTree()));
    // after computing expression, extend the expression result
    mem->signext_id = reg++;

    mem->array_reg_id = table.find(*ident)->second;

    // after compute exp, we will getelementptr to get the address, store into reg_id, that is a[exp]'s address.
    mem->reg_id = reg++;

    if (mem->mem_type == VariableType(Array)) {
      // get the size of array
      mem->size = array_size_table.find(*ident)->second;
    }
  } else if (exp_list1_ast == nullptr) {  // array or pointer as function parameter passing
    mem->exp = nullptr;
    // reg_id is the address of the array
    mem->reg_id = table.find(*ident)->second;
    if (mem->mem_type == VariableType(Array)) {
      // get the size of array
      mem->size = array_size_table.find(*ident)->second;
    }
  }

  return mem;
}

BaseIr *ExpAST::buildIrTree() {
  if (l_or_exp_ast) {
    return l_or_exp_ast->buildIrTree();
  } else if (str) {
    // The expression is a const string
    auto const_str = new ConstExp();
    const_str->const_type = ConstType(CStr);
    // original string begins with " and end with ", delete " symbol of this string
    const_str->str = str->substr(1, str->length() - 2);

    if (std::find(BaseIr::const_strings.begin(), BaseIr::const_strings.end(), const_str->str) ==
        BaseIr::const_strings.end()) {
      BaseIr::const_strings.push_back(const_str->str);
    }
    return const_str;
  } else if (l_val) {  // for &a return a MemExp
    return l_val->buildIrTree();
  }
  return nullptr;
}

int ExpAST::getExpNum() {
  assert(l_or_exp_ast != nullptr);
  // only work for const expression, to get the const number of this long tree
  auto or_exp = dynamic_cast<LOrExpAST *>(l_or_exp_ast.get());
  auto and_exp = dynamic_cast<LAndExpAST *>(or_exp->l_and_exp_ast.get());
  auto eq_exp = dynamic_cast<EqExpAST *>(and_exp->eq_exp_ast.get());
  auto rel_exp = dynamic_cast<RelExpAST *>(eq_exp->rel_exp_ast.get());
  auto add_exp = dynamic_cast<AddExpAST *>(rel_exp->add_exp_ast.get());
  // here is unary, not mul: look MulExp in parser.y
  auto unary_exp = dynamic_cast<UnaryExpAST *>(add_exp->mul_exp_ast.get());
  auto primary_exp = dynamic_cast<PrimaryExpAST *>(unary_exp->primary_exp_ast.get());
  auto number = dynamic_cast<NumberAST *>(primary_exp->number_ast.get());

  return number->int_const;

  // auto ret_ir = l_or_exp_ast->buildIrTree();
  // auto exp_ir = dynamic_cast<ExpIr *>(ret_ir);
  // assert(exp_ir != nullptr);

  // if (exp_ir->exp_type != ExpType(Const)) {
  //   std::cout << "exp is not a const" << std::endl;
  //   assert(0);
  // } else {
  //   int value = dynamic_cast<ConstExp *>(exp_ir)->value;
  //   delete ret_ir;
  //   return value;
  // }
}

BaseIr *LOrExpAST::buildIrTree() {
  if (l_or_exp_rule == 0) {
    return l_and_exp_ast->buildIrTree();
  } else if (l_or_exp_rule == 1) {
    auto binop = new AndOrBinopExp();
    // binop->id = ir_id++;
    // binop->type = IrType(Exp);

    binop->op = BinOpType(Or);
    binop->last_block_label = all_label[all_label.size() - 1];
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(l_or_exp_ast->buildIrTree()));

    // first bool res
    binop->bool_res1 = reg++;
    binop->label1 = reg++;
    all_label.push_back(binop->label1);

    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(l_and_exp_ast->buildIrTree()));
    // second bool res
    binop->bool_res2 = reg++;
    binop->label2 = reg++;
    all_label.push_back(binop->label2);
    // jsa
    binop->bool_result_reg = reg++;
    binop->reg_id = reg++;

    // last_block_label

    return binop;
  }
}

BaseIr *LAndExpAST::buildIrTree() {
  // switch (l_and_exp_rule) {
  if (l_and_exp_rule == 0) {
    return eq_exp_ast->buildIrTree();
  } else if (l_and_exp_rule == 1) {
    auto binop = new AndOrBinopExp();
    // binop->id = ir_id++;
    // binop->type = IrType(Exp);

    binop->op = BinOpType(And);
    // binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(l_and_exp_ast->buildIrTree()));
    // binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(eq_exp_ast->buildIrTree()));
    // jsa
    // binop->bool_result_reg = reg++;
    // binop->reg_id = reg++;

    binop->last_block_label = all_label[all_label.size() - 1];
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(l_and_exp_ast->buildIrTree()));

    // first bool res
    binop->bool_res1 = reg++;
    binop->label1 = reg++;
    all_label.push_back(binop->label1);

    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(eq_exp_ast->buildIrTree()));
    // second bool res
    binop->bool_res2 = reg++;
    binop->label2 = reg++;
    all_label.push_back(binop->label2);
    // jsa
    binop->bool_result_reg = reg++;
    binop->reg_id = reg++;

    return binop;
  }
}
/*
  rel_exp_rule = 0 : RelExp
  rel_exp_rule = 1 : EqExp EQ RelExp
  rel_exp_rule = 2 : EqExp NE RelExp
*/
BaseIr *EqExpAST::buildIrTree() {
  if (eq_rule == 0) {
    return rel_exp_ast->buildIrTree();
  } else if (eq_rule == 1) {
    auto binop = new NorBoolBinopExp();

    binop->op = BinOpType(Eq);
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(eq_exp_ast->buildIrTree()));
    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(rel_exp_ast->buildIrTree()));
    binop->bool_result_reg = reg++;
    binop->reg_id = reg++;
    return binop;
  } else if (eq_rule == 2) {
    auto binop = new NorBoolBinopExp();

    binop->op = BinOpType(Ne);
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(eq_exp_ast->buildIrTree()));
    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(rel_exp_ast->buildIrTree()));
    binop->bool_result_reg = reg++;
    binop->reg_id = reg++;
    return binop;
  }
}

BaseIr *RelExpAST::buildIrTree() {
  if (rel_exp_rule == 0) {  // AddExp
    return add_exp_ast->buildIrTree();
  } else if (rel_exp_rule == 1) {  // RelExp LT AddExp
    auto binop = new NorBoolBinopExp();

    binop->op = BinOpType(Lt);
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(rel_exp_ast->buildIrTree()));
    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(add_exp_ast->buildIrTree()));
    binop->bool_result_reg = reg++;
    binop->reg_id = reg++;

    return binop;
  } else if (rel_exp_rule == 2) {  // RelExp GT AddExp
    auto binop = new NorBoolBinopExp();

    binop->op = BinOpType(Gt);
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(rel_exp_ast->buildIrTree()));
    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(add_exp_ast->buildIrTree()));
    binop->bool_result_reg = reg++;
    binop->reg_id = reg++;
    return binop;
  } else if (rel_exp_rule == 3) {  // RelExp LE AddExp
    auto binop = new NorBoolBinopExp();

    binop->op = BinOpType(Le);
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(rel_exp_ast->buildIrTree()));
    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(add_exp_ast->buildIrTree()));
    binop->bool_result_reg = reg++;
    binop->reg_id = reg++;
    return binop;
  } else if (rel_exp_rule == 4) {  // RelExp GE AddExp
    auto binop = new NorBoolBinopExp();

    binop->op = BinOpType(Ge);
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(rel_exp_ast->buildIrTree()));
    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(add_exp_ast->buildIrTree()));
    binop->bool_result_reg = reg++;
    binop->reg_id = reg++;
    return binop;
  } else {
    return nullptr;
  }
}

BaseIr *AddExpAST::buildIrTree() {
  if (add_exp_rule == 0) {
    return mul_exp_ast->buildIrTree();
  } else if (add_exp_rule == 1) {
    auto binop = new BinopExp();
    // binop->id = ir_id++;
    // binop->type = IrType(Exp);

    binop->op = BinOpType(Add);
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(add_exp_ast->buildIrTree()));
    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(mul_exp_ast->buildIrTree()));
    binop->reg_id = reg++;

    return binop;
  } else if (add_exp_rule == 2) {
    auto binop = new BinopExp();
    // binop->id = ir_id++;
    // binop->type = IrType(Exp);

    binop->op = BinOpType(Minus);
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(add_exp_ast->buildIrTree()));
    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(mul_exp_ast->buildIrTree()));
    binop->reg_id = reg++;

    return binop;
  }
}

BaseIr *UnaryExpAST::buildIrTree() {
  if (primary_exp_ast != nullptr) {
    return primary_exp_ast->buildIrTree();
  } else if (ident != nullptr) {  // function call
    auto func_call = new CallExp();
    if (func_rettype_table.find(*ident) != func_rettype_table.end()) {  // function exist
      // set function return type
      func_call->ret_type = func_rettype_table.find(*ident)->second;
      // set function name
      func_call->name = *ident;

      // set function parameter
      if (func_rparams_ast != nullptr) {  // with parameters
        auto para_list = dynamic_cast<FuncRParamsAST *>(func_rparams_ast.get());

        // first parameter
        func_call->params.push_back(std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(para_list->exp_ast->buildIrTree())));

        // other parameter
        auto exp_list = dynamic_cast<ExpList2AST *>((para_list->exp_list2_ast).get());
        while (exp_list != nullptr) {
          func_call->params.push_back(std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(exp_list->exp_ast->buildIrTree())));

          exp_list = dynamic_cast<ExpList2AST *>((exp_list->exp_list2_ast).get());
        }
      }

      if (func_call->ret_type == VariableType(Int)) {
        func_call->reg_id = reg++;
      }
    } else {
      std::cout << "function:" << *ident << "doesn't exist" << std::endl;
      assert(0);
    }

    return func_call;
  } else if (unary_exp_ast != nullptr) {
    return nullptr;
  }
}

BaseIr *PrimaryExpAST::buildIrTree() {
  if (exp_ast) {
    // that is (EXP)

    return exp_ast->buildIrTree();
  } else if (l_val_ast) {
    auto temp = new TempExp();
    // temp->id = ir_id++;
    // temp->type = IrType(Exp);

    temp->exp_type = ExpType(Temp);

    temp->mem = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(l_val_ast->buildIrTree()));
    temp->reg_id = reg++;

    auto mem_exp = dynamic_cast<MemExp *>(temp->mem.get());
    assert(mem_exp != nullptr);

    if (mem_exp->mem_type == VariableType(Int)) {  // simple int
      temp->res_type = VariableType(Int);
    } else if (mem_exp->exp != nullptr) {  // a[i]
      temp->res_type = VariableType(Int);
    } else {  // array a in function params passing
      temp->res_type = VariableType(Pointer);
    }
    return temp;
  } else if (number_ast) {
    return number_ast->buildIrTree();
  }
}

BaseIr *NumberAST::buildIrTree() {
  auto const_exp = new ConstExp();

  // const_exp->reg_id = reg++;
  // const_exp->id = ir_id++;
  const_exp->exp_type = ExpType(Const);

  const_exp->value = int_const;
  return const_exp;
}
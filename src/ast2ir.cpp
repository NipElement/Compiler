#include <assert.h>
#include <memory>
#include <unordered_map>
#include "ast.hpp"
#include "irtree.hpp"
// todo for array
std::unordered_map<std::string, int> array_size_table;
std::unordered_map<std::string, int> table;
std::unordered_map<std::string, VariableType> variable_type_table;

// id is for printTree
int ir_id = 0;
// reg is the $..
int reg = 0;
BaseIr *StartRoot::buildIrTree() {
  auto ir_root = new RootIr();
  ir_root->id = ir_id++;
  auto p = dynamic_cast<CompUnitAST *>(comp_unit_ast.get());

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

  auto func_def = new FuncDefIr();
  func_def->type = IrType(FuncDef);
  func_def->id = ir_id++;
  if (type == 0) {
    // int
    func_def->ret_type = FuncDefIr::RetType(FuncDefIr::INT);
  } else {
    // void
    func_def->ret_type = FuncDefIr::RetType(FuncDefIr::VOID);
  }

  auto p = dynamic_cast<FuncFParamsAST *>(func_fparams_ast.get());

  // generate parameter list
  if (p) {
    // it must have a child ?
    assert(p->func_fparam_ast != nullptr);
    auto first_para = dynamic_cast<FuncFParamAST *>(p->func_fparam_ast.get());

    func_def->param_types.push_back(VariableType(first_para->type));
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
  // generate block
  func_def->block = std::unique_ptr<BaseIr>(dynamic_cast<BlockAST *>(block_ast.get())->buildIrTree());

  dynamic_cast<BlockIr *>(func_def->block.get())->param_num = func_def->param_names.size();

  // if the function returns void, add a return stmt to block
  if (type == FuncDefIr::RetType(FuncDefIr::VOID)) {
    auto ret_stmt = new RetIr();
    ret_stmt->id = ir_id++;
    ret_stmt->type = IrType(Ret);
    ret_stmt->ret_value = nullptr;

    dynamic_cast<BlockIr *>(func_def->block.get())->stmts.push_back(std::unique_ptr<BaseIr>(ret_stmt));
  }
  return func_def;
}

BaseIr *BlockAST::buildIrTree() {
  auto block = new BlockIr();
  block->type = IrType(Block);
  block->id = ir_id++;

  auto p = dynamic_cast<BlockItemListAST *>(block_item_list_ast.get());
  // for every block item, add it to block->stmts
  while (p) {
    auto block_item_leaf = dynamic_cast<BlockItemAST *>(p->block_item_ast.get());

    block->stmts.push_back(std::unique_ptr<BaseIr>(block_item_leaf->buildIrTree()));

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
      decl->type = IrType(VarDec);
      decl->id = ir_id++;
      decl->mem_id = reg++;
      table.insert(std::pair<std::string, int>(*var_def->ident, decl->mem_id));
      variable_type_table.insert(std::pair<std::string, VariableType>(*var_def->ident, VariableType(Array)));

      return decl;
    } else {  // array
      auto decl = new VarDeclIr();
      decl->var_type = VariableType(Array);

      decl->type = IrType(VarDec);
      decl->id = ir_id++;
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
    // here only handle stmt = 0
    auto stmt = dynamic_cast<StmtAST *>(stmt_ast.get());
    return stmt->buildIrTree();
  }
}

BaseIr *StmtAST::buildIrTree() {
  if (stmt_rule == 0) {
    auto move = new MoveIr();
    move->id = ir_id++;
    move->type = IrType(Move);

    // in fact this is a move ir
    auto l_val = dynamic_cast<LValAST *>(l_val_ast.get());
    auto exp = dynamic_cast<ExpAST *>(exp_ast.get());
    move->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(l_val->buildIrTree()));
    move->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(exp->buildIrTree()));
    return move;
  } else if (stmt_rule == 3) {
    return block_ast->buildIrTree();
  } else if (stmt_rule == 5) {
    auto cjump = new CjumpIr();
    cjump->id = ir_id++;
    cjump->type = IrType(Cjump);

    cjump->exp = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(exp_ast->buildIrTree()));

    cjump->condition_reg = reg++;
    cjump->t = reg++;
    cjump->t_block = std::unique_ptr<BaseIr>(stmt1_ast->buildIrTree());
    cjump->f = reg++;
    cjump->f_block = std::unique_ptr<BaseIr>(stmt2_ast->buildIrTree());
    cjump->done = reg++;
    return cjump;
  } else
    return nullptr;
}

BaseIr *LValAST::buildIrTree() {
  auto mem = new MemExp();
  mem->id = ir_id++;
  mem->type = IrType(Exp);
  mem->exp_type = ExpType(Mem);
  if (exp_list1_ast == nullptr) {  // simple variable int
    mem->signext_id = -1;
    mem->exp = nullptr;
    // the address of this lval
    mem->reg_id = table.find(*ident)->second;

    // look up the table to set type info
    mem->mem_type = variable_type_table.find(*ident)->second;

  } else {  // array or pointer with exp
    // set type info
    mem->mem_type = variable_type_table.find(*ident)->second;
    if (mem->mem_type == VariableType(Pointer)) {
      mem->pointer_value_reg_id = reg++;
    }

    mem->signext_id = reg++;
    // a[exp]..
    auto exp_list1 = dynamic_cast<ExpList1AST *>(exp_list1_ast.get());

    // suppose no exp combined by ','
    assert(exp_list1->exp_list1_ast == nullptr);

    auto exp = dynamic_cast<ExpAST *>(exp_list1->exp_ast.get());

    mem->exp = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(exp->buildIrTree()));
    mem->reg_id = table.find(*ident)->second;
    // after compute exp, we will getelementptr to get the address, store into ele_reg_id
    mem->ele_reg_id = reg++;

    if (mem->mem_type == VariableType(Array)) {
      // get the size of array
      mem->size = array_size_table.find(*ident)->second;
    }
  }

  return mem;
}

BaseIr *ExpAST::buildIrTree() { return l_or_exp_ast->buildIrTree(); }

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
    auto binop = new BinopExp();
    binop->id = ir_id++;
    binop->type = IrType(Exp);

    binop->op = BinOpType(Or);
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(l_and_exp_ast->buildIrTree()));
    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(l_or_exp_ast->buildIrTree()));

    return binop;
  }
}

BaseIr *LAndExpAST::buildIrTree() {
  switch (l_and_exp_rule) {
    case 0:
      return eq_exp_ast->buildIrTree();
    case 1:
      return nullptr;
    default:
      break;
  }
}

BaseIr *EqExpAST::buildIrTree() {
  switch (eq_rule) {
    case 0:
      return rel_exp_ast->buildIrTree();
    case 1:
      return nullptr;
    default:
      break;
  }
}

BaseIr *RelExpAST::buildIrTree() {
  switch (rel_exp_rule) {
    case 0:
      return add_exp_ast->buildIrTree();
      break;
    case 1:
    // TODO ...
    default:
      break;
  }
}

BaseIr *AddExpAST::buildIrTree() {
  if (add_exp_rule == 0) {
    return mul_exp_ast->buildIrTree();
  } else if (add_exp_rule == 1) {
    auto binop = new BinopExp();
    binop->id = ir_id++;
    binop->type = IrType(Exp);

    binop->op = BinOpType(Add);
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(mul_exp_ast->buildIrTree()));
    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(add_exp_ast->buildIrTree()));
    binop->reg_id = reg++;

    return binop;
  } else if (add_exp_rule == 2) {
    auto binop = new BinopExp();
    binop->id = ir_id++;
    binop->type = IrType(Exp);
    binop->reg_id = reg++;

    binop->op = BinOpType(Minus);
    binop->exp1 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(mul_exp_ast->buildIrTree()));
    binop->exp2 = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(add_exp_ast->buildIrTree()));

    return binop;
  }
}

BaseIr *UnaryExpAST::buildIrTree() {
  if (primary_exp_ast != nullptr) {
    return primary_exp_ast->buildIrTree();
  } else if (func_rparams_ast != nullptr) {
    return nullptr;
  } else if (unary_exp_ast != nullptr) {
    return nullptr;
  }
}

BaseIr *PrimaryExpAST::buildIrTree() {
  if (exp_ast) {
    return nullptr;
  } else if (l_val_ast) {
    auto temp = new TempExp();
    temp->id = ir_id++;
    temp->type = IrType(Exp);
    temp->exp_type = ExpType(Temp);
    temp->reg_id = reg++;

    temp->mem = std::unique_ptr<ExpIr>(dynamic_cast<ExpIr *>(l_val_ast->buildIrTree()));
    return temp;
  } else if (number_ast) {
    return number_ast->buildIrTree();
  }
}
BaseIr *NumberAST::buildIrTree() {
  auto const_exp = new ConstExp();
  const_exp->reg_id = reg++;
  const_exp->id = ir_id++;
  const_exp->exp_type = ExpType(Const);

  const_exp->value = int_const;
  return const_exp;
}
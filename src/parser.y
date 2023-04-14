%code requires {
  #include <memory>
  #include <string>
  #include <ast.hpp>
}

%{

#include <iostream>
#include <memory>
#include <string>
#include <ast.hpp>

int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

int id=0;
%}

%parse-param { std::unique_ptr<BaseAST> &ast }

%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}

 
%token INT RETURN VOID CONST IF ELSE WHILE BREAK CONTINUE GE LE GT LT EQ NE AND OR
%token <str_val> IDENT
%token <int_val> INT_CONST 


%type <ast_val> StartRoot CompUnit FuncDef Block Stmt Number Decl ConstDecl VarDecl 
ConstDef ConstDefList ConstInitVal ConstExp ConstInitValList 
VarDef VarDefList InitVal InitValList FuncFParams FuncFParamList FuncFParam 
BlockItemList BlockItem LVal LOrExp ExpList1 PrimaryExp UnaryExp UnaryOp FuncRParams 
ExpList2 MulExp AddExp RelExp EqExp LAndExp 

%%

StartRoot
  : CompUnit {
    auto start_root = new StartRoot();
    start_root->comp_unit_ast = std::unique_ptr<BaseAST>($1);
    start_root->id = id++;
    ast = move(start_root);
    //std::cout<<"CompUnit"<<endl;
  }
  ;

CompUnit
  : CompUnit Decl {
      // auto ast = new CompUnit();
      // ast->comp_unit_ast = unique_ptr<BaseAST>($1);
      // ast->func_def_ast= nullptr;
      // ast->decl_ast = unique_ptr<BaseAST>($2);
      // $$ = ast;
  }
  | CompUnit FuncDef {
    auto ast = new CompUnit();
    ast->comp_unit_ast = unique_ptr<BaseAST>($1);
    ast->decl_ast = nullptr;
    ast->func_def_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
    $$ = ast;
    //std::cout<<"CompUnit FuncDef"<<endl;
  }
  | {
      $$ = nullptr;
  }
  ;

Decl
  : ConstDecl {
      // auto ast = new Decl();
      // ast->const_decl_ast = unique_ptr<BaseAST>($1);
      // ast->var_decl_ast = nullptr;
      // $$ = ast;
  }
  | VarDecl {
      // auto ast = new Decl();
      // ast->const_decl_ast = nullptr;
      // ast->var_decl_ast = unique_ptr<BaseAST>($1);
      // $$ = ast; 
  }
  ;

//ConstDecl     ::= "const" INT ConstDef {"," ConstDef} ";"
ConstDecl
  : CONST INT ConstDef ConstDefList ';'{
    
  }
  ;

ConstDefList
  :
  | ',' ConstDef ConstDefList{
    std::cout<<"',' ConstDef ConstDefList"<<endl;
  }
  
  ;

ConstDef
  : IDENT ExpList1 '=' ConstInitVal{
    std::cout<<"IDENT ExpList '=' ConstInitVal"<<endl;
  } 
  ;
/*

%token ID
%%
s: a ID
a: expr
expr: %empty | expr ID ','

*/
//ConstInitVal  ::= ConstExp | "{" [ConstInitVal {"," ConstInitVal}] "}";
ConstInitVal
  : ConstExp {
    std::cout<<"ConstExp"<<endl;
  }
  | '{'  '}'{
    std::cout<<"'{'  '}'"<<endl;
  }
  | '{' ConstInitVal ConstInitValList '}' {
    std::cout<<"'{' ConstInitVal ConstInitValList '}'"<<endl;
  }
  ;

ConstInitValList
  : 
  | ',' ConstInitVal ConstInitValList{
    std::cout<<"',' ConstInitVal ConstInitValList"<<endl;
  }
  
  ;

VarDecl
  : INT VarDef VarDefList ';' {
    auto ast = new VarDecl();
    ast->type = 0;
    ast->var_def_ast = unique_ptr<BaseAST>($2);
    ast->var_def_list_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
    //std::cout<<"INT VarDef VarDefList ';'"<<endl;
  }
  ;

VarDefList
  : 
  |  ',' VarDef VarDefList {
    auto ast = new VarDefList();
    ast->var_def_ast = unique_ptr<BaseAST>($2);
    ast->var_def_list_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
    //std::cout<<"VarDefList ',' VarDef"<<endl;
  }
  ;

VarDef 
  : IDENT ExpList1 {
    auto ast = new VarDef();
    ast->ident = $1;
    ast->exp_list1_ast = unique_ptr<BaseAST>($2);
    ast->initval_ast = nullptr;
    ast->id = id++;
    //std::cout<<"IDENT ExpList"<<endl;
  }
  | IDENT ExpList1 '=' InitVal{
    // auto ast = new VarDef();
    // ast->ident = $1;
    // ast->exp_list1_ast = unique_ptr<BaseAST>($2);
    // ast->initval_ast = unique_ptr<BaseAST>($4);
    std::cout<<"IDENT ExpList '=' InitVal"<<endl;
  }
  ;

InitVal
  : Exp
  | '{'  '}'{
    std::cout<<"'{'  '}'"<<endl;
  }
  | '{' InitVal InitValList '}'{
    std::cout<<"'{' InitVal InitValList '}'"<<endl;
  }
  ;

InitValList
  : 
  | ',' InitVal InitValList{
    std::cout<<"',' InitVal InitValList"<<endl;
  }
  ;

FuncDef
  : INT IDENT '(' ')' Block {
    std::cout<<"INT IDENT '(' ')' Block"<<endl;
  }
  | VOID IDENT '(' ')' Block {
    std::cout<<"INT IDENT '(' ')' Block"<<endl;
  }
  | INT IDENT '(' FuncFParams ')' Block {

    std::cout<<"FuncType IDENT '(' FuncFParams ')' Block"<<endl;
  }
  | VOID IDENT '(' FuncFParams ')' Block {
    auto ast = new FuncDef();
    ast->type = 1;
    ast->ident = $2;
    ast->func_fparams_ast = unique_ptr<BaseAST>($4);
    ast->block_ast = unique_ptr<BaseAST>($6);
    ast->id = id++;
    //std::cout<<"FuncType IDENT '(' FuncFParams ')' Block"<<endl;
  }
  ;

FuncFParams
  : FuncFParam FuncFParamList {
    auto ast = new FuncFParams();
    ast->func_fparam_ast = unique_ptr<BaseAST>($1);
    ast->func_fparam_list_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
    //std::cout<<"FuncFParam FuncFParamsList"<<endl;
  }
  ;

FuncFParamList
  : {
    $$ = nullptr;
  }
  | ',' FuncFParam FuncFParamList {
    auto ast = new FuncFParams();
    ast->func_fparam_ast = unique_ptr<BaseAST>($1);
    ast->func_fparam_list_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
    //std::cout<<"',' FuncFParam FuncFParamsList"<<endl;
  }
  ;

FuncFParam
  : INT IDENT {
    auto ast = new FuncFParam();
    ast->type = 0;
    ast->ident = $2;
    ast->exp_list1_ast = nullptr;
    ast->id = id++;
    //std::cout<<"INT IDENT"<<endl;
  }
  | INT IDENT '['  ']' ExpList1 {
    auto ast = new FuncFParam();
    ast->type = 0;
    ast->ident = $2;
    ast->exp_list1_ast = unique_ptr<BaseAST>($5);
    ast->id = id++;
    //std::cout<<"INT IDENT '['  ']' ExpList"<<endl;
  }
  ;

Block
  : '{' BlockItemList '}' {
    auto ast = new Block();
    ast->block_item_list_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
    //std::cout<<"'{' BlockItemList '}'"<<endl;
  }
  ;

BlockItemList
  : {
    $$ = nullptr;
  }
  | BlockItem BlockItemList {
    auto ast = new BlockItemList();
    ast->block_item_ast = unique_ptr<BaseAST>($1);
    ast->block_item_list_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
    //std::cout<<"BlockItem BlockItemList"<<endl;
  }  
  ;

BlockItem
  : Decl {
    auto ast = new BlockItem();
    ast->decl_ast = unique_ptr<BaseAST>($1);
    ast->stmt_ast = nullptr;
    ast->id = id++;
    //std::cout<<"Decl"<<endl;
  }
  | Stmt {
    auto ast = new BlockItem();
    ast->decl_ast = nullptr;
    ast->stmt_ast = unique_ptr<BaseAST>($1);
    ast->id = id++;
    //std::cout<<"Stmt"<<endl;
  }
  ;

Stmt
  : LVal '=' Exp ';' {
    auto ast = new Stmt();
    ast->stmt_rule = 0;
    ast->l_val_ast = unique_ptr<BaseAST>($1);
    ast->exp_ast = unique_ptr<BaseAST>($3);
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
  }
  | ';' {
    auto ast = new Stmt();
    ast->stmt_rule = 1;
    ast->l_val_ast = nullptr;
    ast->exp_ast = nullptr;
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
  }
  | Exp ';' {
    auto ast = new Stmt();
    ast->stmt_rule = 2;
    ast->l_val_ast = nullptr;
    ast->exp_ast = unique_ptr<BaseAST>($1);
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
  }
  | Block {
    auto ast = new Stmt();
    ast->id = id++;
    ast->stmt_rule = 3;
    ast->l_val_ast = nullptr;
    ast->exp_ast = nullptr;
    ast->block_ast = unique_ptr<BaseAST>($1);
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
  }
  | IF '(' Exp ')' Stmt {
    auto ast = new Stmt();
    ast->stmt_rule = 4;
    ast->l_val_ast = nullptr;
    ast->exp_ast = unique_ptr<BaseAST>($3);
    ast->block_ast = nullptr;
    ast->stmt1_ast = unique_ptr<BaseAST>($5);
    ast->stmt2_ast = nullptr;
    ast->id = id++;
  }
  | IF '(' Exp ')' Stmt ELSE Stmt {
    auto ast = new Stmt();
    ast->stmt_rule = 5;
    ast->l_val_ast = nullptr;
    ast->exp_ast = unique_ptr<BaseAST>($3);
    ast->block_ast = nullptr;
    ast->stmt1_ast = unique_ptr<BaseAST>($5);
    ast->stmt2_ast = unique_ptr<BaseAST>($7);
    ast->id = id++;
  }
  | WHILE '(' Exp ')' Stmt {
    auto ast = new Stmt();
    ast->stmt_rule = 6;
    ast->l_val_ast = nullptr;
    ast->exp_ast = unique_ptr<BaseAST>($3);
    ast->block_ast = nullptr;
    ast->stmt1_ast = unique_ptr<BaseAST>($5);
    ast->stmt2_ast = nullptr;
    ast->id = id++;
  }
  | BREAK ';' {
    auto ast = new Stmt();
    ast->stmt_rule = 7;
    ast->l_val_ast = nullptr;
    ast->exp_ast = nullptr;
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
  }
  | CONTINUE ';' {
    auto ast = new Stmt();
    ast->stmt_rule = 8;
    ast->l_val_ast = nullptr;
    ast->exp_ast = nullptr;
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
  }
  | RETURN ';' {
    auto ast = new Stmt();
    ast->stmt_rule = 9;
    ast->l_val_ast = nullptr;
    ast->exp_ast = nullptr;
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
  }
  | RETURN Exp ';' {
    auto ast = new Stmt();
    ast->stmt_rule = 10;
    ast->l_val_ast = nullptr;
    ast->exp_ast = unique_ptr<BaseAST>($2);
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
  }
  ;

Exp
  : LOrExp {
    auto ast = new Exp();
    ast->l_or_exp_ast = unique_ptr<BaseAST>($1);
    ast->id = id++;
  }
  ;

LVal
  : IDENT ExpList1 {
    auto ast = new LVal();
    ast->ident = $1;
    ast->exp_list1_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
  }
  ;

ExpList1
  : {
    $$ = nullptr;
  }
  | '[' Exp ']' ExpList1 {
    auto ast = new ExpList1();
    ast->exp_ast = unique_ptr<BaseAST>($2);
    ast->exp_list1_ast = unique_ptr<BaseAST>($4);
    ast->id = id++;
  }
  ;

PrimaryExp    
  : '(' Exp ')' {
    auto ast = new PrimaryExp();
    ast->exp_ast = unique_ptr<BaseAST>($2);
    ast->l_val_ast = nullptr;
    ast->number_ast = nullptr;
    ast->id = id++;
  }
  | LVal {
    auto ast = new PrimaryExp();
    ast->exp_ast = nullptr;
    ast->l_val_ast = unique_ptr<BaseAST>($1);
    ast->number_ast = nullptr;
    ast->id = id++;
  }
  | Number {
    auto ast = new PrimaryExp();
    ast->exp_ast = nullptr;
    ast->l_val_ast = nullptr;
    ast->number_ast = unique_ptr<BaseAST>($1);
    ast->id = id++;
  }
  ;

Number
  : INT_CONST {
    auto ast = new PrimaryExp();
    ast->exp_int_constast = $1;
    ast->id = id++;
    //std::cout<<"INT_CONST"<<endl;
  }
  ;

UnaryExp 
  : PrimaryExp {
    auto ast = new UnaryExp();
    ast->ident = "";
    ast->primary_exp_ast = unique_ptr<BaseAST>($1);
    ast->func_rparams_ast = nullptr;
    ast->unary_op_ast = nullptr;
    ast->unary_exp_ast = nullptr;
    ast->id = id++;
  }
  | IDENT '(' ')' {
    auto ast = new UnaryExp();
    ast->ident = $1;
    ast->primary_exp_ast = nullptr;
    ast->func_rparams_ast = nullptr;
    ast->unary_op_ast = nullptr;
    ast->unary_exp_ast = nullptr;
    ast->id = id++;
  }
  | IDENT '(' FuncRParams ')' {
    auto ast = new UnaryExp();
    ast->ident = $1;
    ast->primary_exp_ast = nullptr;
    ast->func_rparams_ast = unique_ptr<BaseAST>($3);
    ast->unary_op_ast = nullptr;
    ast->unary_exp_ast = nullptr;
    ast->id = id++;
  }
  | UnaryOp UnaryExp {
    auto ast = new UnaryExp();
    ast->ident = "";
    ast->primary_exp_ast = nullptr;
    ast->func_rparams_ast = nullptr;
    ast->unary_op_ast = unique_ptr<BaseAST>($1);
    ast->unary_exp_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
  }
  ;

UnaryOp
  : '+' {
    auto ast = new UnaryOp();
    ast->unary_op_rule = 0;
    ast->id = id++;
  }
  | '-' {
    auto ast = new UnaryOp();
    ast->unary_op_rule = 1;
    ast->id = id++;
  }
  | '!' {
    auto ast = new UnaryOp();
    ast->unary_op_rule = 2;
    ast->id = id++;
  }
  ;

FuncRParams
  : Exp ExpList2 {
    auto ast = new FuncRParams();
    ast->exp_ast = unique_ptr<BaseAST>($1);
    ast->exp_list2_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
  }
  ;

ExpList2
  : {
    $$ = nullptr;
  }
  | ',' Exp ExpList2 {
    auto ast = new ExpList2();
    ast->exp_ast = unique_ptr<BaseAST>($2);
    ast->exp_list2_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
  }
  ;

MulExp
  : UnaryExp
  | MulExp '*' UnaryExp
  | MulExp '/' UnaryExp
  | MulExp '%' UnaryExp
  ;

AddExp
  : MulExp {
    auto ast = new AddExp();
    ast->add_exp_rule = 0;
    ast->mul_exp_ast = unique_ptr<BaseAST>($1);
    ast->add_exp_ast = nullptr;
    ast->id = id++;
  }
  | AddExp '+' MulExp {
    auto ast = new AddExp();
    ast->add_exp_rule = 1;
    ast->mul_exp_ast = unique_ptr<BaseAST>($1);
    ast->add_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
  }
  | AddExp '-' MulExp {
    auto ast = new AddExp();
    ast->add_exp_rule = 2;
    ast->mul_exp_ast = unique_ptr<BaseAST>($1);
    ast->add_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
  }
  ;

RelExp
  : AddExp {
    auto ast = new RelExp();
    ast->add_exp_rule = 0;
    ast->add_exp_ast = unique_ptr<BaseAST>($1);
    ast->rel_exp_ast = nullptr;
    ast->id = id++;
  }
  | RelExp LT AddExp {
    auto ast = new RelExp();
    ast->add_exp_rule = 1;
    ast->add_exp_ast = unique_ptr<BaseAST>($1);
    ast->rel_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
  }
  | RelExp GT AddExp {
    auto ast = new RelExp();
    ast->add_exp_rule = 2;
    ast->add_exp_ast = unique_ptr<BaseAST>($1);
    ast->rel_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
  }
  | RelExp LE AddExp {
    auto ast = new RelExp();
    ast->add_exp_rule = 3;
    ast->add_exp_ast = unique_ptr<BaseAST>($1);
    ast->rel_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
  }
  | RelExp GE AddExp {
    auto ast = new RelExp();
    ast->add_exp_rule = 4;
    ast->add_exp_ast = unique_ptr<BaseAST>($1);
    ast->rel_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
  }
  ;

EqExp
  : RelExp {
    auto ast = new EqExp();
    ast->add_exp_rule = 0;
    ast->eq_exp_ast = nullptr;
    ast->rel_exp_ast = unique_ptr<BaseAST>($1);
    ast->id = id++;
  }
  | EqExp EQ RelExp {
    auto ast = new EqExp();
    ast->add_exp_rule = 1;
    ast->eq_exp_ast = unique_ptr<BaseAST>($1);
    ast->rel_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
  }
  | EqExp NE RelExp {
    auto ast = new EqExp();
    ast->add_exp_rule = 2;
    ast->eq_exp_ast = unique_ptr<BaseAST>($1);
    ast->rel_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
  }
  ;

LAndExp
  : EqExp {
    auto ast = new EqExp();
    ast->add_exp_rule = 0;
    ast->eq_exp_ast = unique_ptr<BaseAST>($1);
    ast->l_and_exp_ast = nullptr;
    ast->id = id++;
  }
  | LAndExp AND EqExp {
    auto ast = new EqExp();
    ast->add_exp_rule = 1;
    ast->l_and_exp_ast = unique_ptr<BaseAST>($1);
    ast->eq_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
  }
  ;

LOrExp
  : LAndExp {
    auto ast = new EqExp();
    ast->add_exp_rule = 0;
    ast->l_and_exp_ast = unique_ptr<BaseAST>($1);
    ast->l_or_exp_ast = nullptr;
    ast->id = id++;
  }
  | LOrExp OR LAndExp {
    auto ast = new EqExp();
    ast->add_exp_rule = 1;
    ast->l_and_exp_ast = unique_ptr<BaseAST>($1);
    ast->l_or_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
  }
  ;

ConstExp
  : Exp
  ;
%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}

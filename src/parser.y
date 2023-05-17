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
%token <str_val> IDENT STR
%token <int_val> INT_CONST 


%type <ast_val> StartRoot CompUnit FuncDef Block Stmt Number Decl ConstDecl VarDecl 
ConstDef ConstDefList ConstInitVal ConstExp ConstInitValList 
VarDef VarDefList InitVal InitValList FuncFParams FuncFParamList FuncFParam 
BlockItemList BlockItem LVal LOrExp ExpList1 PrimaryExp UnaryExp UnaryOp FuncRParams 
ExpList2 MulExp AddExp RelExp EqExp LAndExp Exp

%%

StartRoot
  : CompUnit {
    auto start_root = make_unique<StartRoot>();;
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
    auto ast = new CompUnitAST();
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
    auto ast = new VarDeclAST();
    ast->type = 0;
    ast->var_def_ast = unique_ptr<BaseAST>($2);
    ast->var_def_list_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
    //std::cout<<"INT VarDef VarDefList ';'"<<endl;
  }
  ;

VarDefList
  : {
    $$ = nullptr;
  }
  |  ',' VarDef VarDefList {
    auto ast = new VarDefListAST();
    ast->var_def_ast = unique_ptr<BaseAST>($2);
    ast->var_def_list_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
    //std::cout<<"VarDefList ',' VarDef"<<endl;
  }
  ;

VarDef 
  : IDENT ExpList1 {
    auto ast = new VarDefAST();
    ast->ident = $1;
    ast->exp_list1_ast = unique_ptr<BaseAST>($2);
    ast->initval_ast = nullptr;
    ast->id = id++;
    $$ = ast;
    // std::cout<<*ast->ident<<std::endl;
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
    auto ast = new FuncDefAST();
    ast->type = 0;
    ast->ident = $2;
    ast->func_fparams_ast = nullptr;
    ast->block_ast = unique_ptr<BaseAST>($5);
    ast->id = id++;
    $$ = ast;
  }
  | VOID IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->type = 1;
    ast->ident = $2;
    ast->func_fparams_ast = nullptr;
    ast->block_ast = unique_ptr<BaseAST>($5);
    ast->id = id++;
    $$ = ast;
  }
  | INT IDENT '(' FuncFParams ')' Block {
    auto ast = new FuncDefAST();
    ast->type = 0;
    ast->ident = $2;
    ast->func_fparams_ast = unique_ptr<BaseAST>($4);
    ast->block_ast = unique_ptr<BaseAST>($6);
    ast->id = id++;
    $$ = ast;
  }
  | VOID IDENT '(' FuncFParams ')' Block {
    auto ast = new FuncDefAST();
    ast->type = 1;
    ast->ident = $2;
    ast->func_fparams_ast = unique_ptr<BaseAST>($4);
    ast->block_ast = unique_ptr<BaseAST>($6);
    ast->id = id++;
    $$ = ast;
    //std::cout<<"FuncType IDENT '(' FuncFParams ')' Block"<<endl;
  }
  ;

FuncFParams
  : FuncFParam FuncFParamList {
    auto ast = new FuncFParamsAST();
    ast->func_fparam_ast = unique_ptr<BaseAST>($1);
    ast->func_fparam_list_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
    $$ = ast;
    //std::cout<<"FuncFParam FuncFParamsList"<<endl;
  }
  ;

FuncFParamList
  : {
    $$ = nullptr;
  }
  | ',' FuncFParam FuncFParamList {
    auto ast = new FuncFParamListAST();
    ast->func_fparam_ast = unique_ptr<BaseAST>($2);
    ast->func_fparam_list_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
    //std::cout<<"',' FuncFParam FuncFParamsList"<<endl;
  }
  ;

FuncFParam
  : INT IDENT {
    auto ast = new FuncFParamAST();
    ast->type = 0;
    ast->ident = $2;
    ast->exp_list1_ast = nullptr;
    ast->id = id++;
    $$ = ast;
    //std::cout<<"INT IDENT"<<endl;
  }
  | INT IDENT '['  ']' ExpList1 {
    auto ast = new FuncFParamAST();
    ast->type = 1;
    ast->ident = $2;
    ast->exp_list1_ast = unique_ptr<BaseAST>($5);
    ast->id = id++;
    $$ = ast;
    //std::cout<<"INT IDENT '['  ']' ExpList"<<endl;
  }
  ;

Block
  : '{' BlockItemList '}' {
    auto ast = new BlockAST();
    ast->block_item_list_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
    $$ = ast;
    //std::cout<<"'{' BlockItemList '}'"<<endl;
  }
  ;

BlockItemList
  : {
    $$ = nullptr;
  }
  | BlockItem BlockItemList {
    auto ast = new BlockItemListAST();
    ast->block_item_ast = unique_ptr<BaseAST>($1);
    ast->block_item_list_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
    $$ = ast;
    //std::cout<<"BlockItem BlockItemList"<<endl;
  }  
  ;

BlockItem
  : Decl {
    auto ast = new BlockItemAST();
    ast->decl_ast = unique_ptr<BaseAST>($1);
    ast->stmt_ast = nullptr;
    ast->id = id++;
    $$ = ast;
    //std::cout<<"Decl"<<endl;
  }
  | Stmt {
    auto ast = new BlockItemAST();
    ast->decl_ast = nullptr;
    ast->stmt_ast = unique_ptr<BaseAST>($1);
    ast->id = id++;
    $$ = ast;
    //std::cout<<"Stmt"<<endl;
  }
  ;

Stmt
  : LVal '=' Exp ';' {
    auto ast = new StmtAST();
    ast->stmt_rule = 0;
    ast->l_val_ast = unique_ptr<BaseAST>($1);
    ast->exp_ast = unique_ptr<BaseAST>($3);
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | ';' {
    auto ast = new StmtAST();
    ast->stmt_rule = 1;
    ast->l_val_ast = nullptr;
    ast->exp_ast = nullptr;
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | Exp ';' {
    auto ast = new StmtAST();
    ast->stmt_rule = 2;
    ast->l_val_ast = nullptr;
    ast->exp_ast = unique_ptr<BaseAST>($1);
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | Block {
    auto ast = new StmtAST();
    ast->id = id++;
    ast->stmt_rule = 3;
    ast->l_val_ast = nullptr;
    ast->exp_ast = nullptr;
    ast->block_ast = unique_ptr<BaseAST>($1);
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | IF '(' Exp ')' Stmt {
    auto ast = new StmtAST();
    ast->stmt_rule = 4;
    ast->l_val_ast = nullptr;
    ast->exp_ast = unique_ptr<BaseAST>($3);
    ast->block_ast = nullptr;
    ast->stmt1_ast = unique_ptr<BaseAST>($5);
    ast->stmt2_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | IF '(' Exp ')' Stmt ELSE Stmt {
    auto ast = new StmtAST();
    ast->stmt_rule = 5;
    ast->l_val_ast = nullptr;
    ast->exp_ast = unique_ptr<BaseAST>($3);
    ast->block_ast = nullptr;
    ast->stmt1_ast = unique_ptr<BaseAST>($5);
    ast->stmt2_ast = unique_ptr<BaseAST>($7);
    ast->id = id++;
    $$ = ast;
  }
  | WHILE '(' Exp ')' Stmt {
    auto ast = new StmtAST();
    ast->stmt_rule = 6;
    ast->l_val_ast = nullptr;
    ast->exp_ast = unique_ptr<BaseAST>($3);
    ast->block_ast = nullptr;
    ast->stmt1_ast = unique_ptr<BaseAST>($5);
    ast->stmt2_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | BREAK ';' {
    auto ast = new StmtAST();
    ast->stmt_rule = 7;
    ast->l_val_ast = nullptr;
    ast->exp_ast = nullptr;
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | CONTINUE ';' {
    auto ast = new StmtAST();
    ast->stmt_rule = 8;
    ast->l_val_ast = nullptr;
    ast->exp_ast = nullptr;
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | RETURN ';' {
    auto ast = new StmtAST();
    ast->stmt_rule = 9;
    ast->l_val_ast = nullptr;
    ast->exp_ast = nullptr;
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | RETURN Exp ';' {
    auto ast = new StmtAST();
    ast->stmt_rule = 10;
    ast->l_val_ast = nullptr;
    ast->exp_ast = unique_ptr<BaseAST>($2);
    ast->block_ast = nullptr;
    ast->stmt1_ast = nullptr;
    ast->stmt2_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  ;

Exp
  : LOrExp {
    auto ast = new ExpAST();
    ast->l_or_exp_ast = unique_ptr<BaseAST>($1);
    ast->id = id++;
    $$ = ast;
  }
  | STR {
    auto ast = new ExpAST();
    ast->l_or_exp_ast = nullptr;
    ast->str= $1;
    ast->id = id++;
    $$ = ast;
  }
  | '&' LVal {
    auto ast = new ExpAST();
    ast->l_or_exp_ast = nullptr;
    ast->str = nullptr;
    ast->l_val = unique_ptr<BaseAST>($2);
    ast->id = id++;
    $$ = ast;
  }
  ;

LVal
  : IDENT ExpList1 {
    auto ast = new LValAST();
    ast->ident = $1;
    ast->exp_list1_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
    $$ = ast;
  }
  ;

ExpList1
  : {
    $$ = nullptr;
  }
  | '[' Exp ']' ExpList1 {
    auto ast = new ExpList1AST();
    ast->exp_ast = unique_ptr<BaseAST>($2);
    ast->exp_list1_ast = unique_ptr<BaseAST>($4);
    ast->id = id++;
    $$ = ast;
    // std::cout<<"array"<<std::endl;
  }
  ;

PrimaryExp    
  : '(' Exp ')' {
    auto ast = new PrimaryExpAST();
    std::cout<<"()"<<std::endl;
    ast->exp_ast = unique_ptr<BaseAST>($2);
    ast->l_val_ast = nullptr;
    ast->number_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | LVal {
    auto ast = new PrimaryExpAST();
    ast->exp_ast = nullptr;
    ast->l_val_ast = unique_ptr<BaseAST>($1);
    ast->number_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | Number {
    auto ast = new PrimaryExpAST();
    ast->exp_ast = nullptr;
    ast->l_val_ast = nullptr;
    ast->number_ast = unique_ptr<BaseAST>($1);
    ast->id = id++;
    $$ = ast;
  }
  ;

Number
  : INT_CONST {
    auto ast = new NumberAST();
    ast->int_const = $1;
    ast->id = id++;
    $$ = ast;
    //std::cout<<"INT_CONST"<<endl;
  }
  ;

UnaryExp 
  : PrimaryExp {
    auto ast = new UnaryExpAST();
    ast->ident = nullptr;
    ast->primary_exp_ast = unique_ptr<BaseAST>($1);
    ast->func_rparams_ast = nullptr;
    ast->unary_op_ast = nullptr;
    ast->unary_exp_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | IDENT '(' ')' {
    auto ast = new UnaryExpAST();
    ast->ident = $1;
    ast->primary_exp_ast = nullptr;
    ast->func_rparams_ast = nullptr;
    ast->unary_op_ast = nullptr;
    ast->unary_exp_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | IDENT '(' FuncRParams ')' {
    auto ast = new UnaryExpAST();
    ast->ident = $1;
    ast->primary_exp_ast = nullptr;
    ast->func_rparams_ast = unique_ptr<BaseAST>($3);
    ast->unary_op_ast = nullptr;
    ast->unary_exp_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | UnaryOp UnaryExp {
    auto ast = new UnaryExpAST();
    ast->ident = nullptr;
    ast->primary_exp_ast = nullptr;
    ast->func_rparams_ast = nullptr;
    ast->unary_op_ast = unique_ptr<BaseAST>($1);
    ast->unary_exp_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
    $$ = ast;
  }
  ;

UnaryOp
  : '+' {
    auto ast = new UnaryOpAST();
    ast->unary_op_rule = 0;
    ast->id = id++;
    $$ = ast;
  }
  | '-' {
    auto ast = new UnaryOpAST();
    ast->unary_op_rule = 1;
    ast->id = id++;
    $$ = ast;
  }
  | '!' {
    auto ast = new UnaryOpAST();
    ast->unary_op_rule = 2;
    ast->id = id++;
    $$ = ast;
  }
  ;

FuncRParams
  : Exp ExpList2 {
    auto ast = new FuncRParamsAST();
    ast->exp_ast = unique_ptr<BaseAST>($1);
    ast->exp_list2_ast = unique_ptr<BaseAST>($2);
    ast->id = id++;
    $$ = ast;
  }
  ;

ExpList2
  : {
    $$ = nullptr;
  }
  | ',' Exp ExpList2 {
    auto ast = new ExpList2AST();
    ast->exp_ast = unique_ptr<BaseAST>($2);
    ast->exp_list2_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
  }
  ;

MulExp
  : UnaryExp {
    auto ast = new MulExpAST();
    ast->mul_exp_rule = 0;
    ast->mul_exp_ast = nullptr;
    ast->unary_exp_ast = unique_ptr<BaseAST>($1);
    ast->id = id++;
    $$ = ast;
  }
  | MulExp '*' UnaryExp{
    auto ast = new MulExpAST();
    ast->mul_exp_rule = 1;
    ast->mul_exp_ast = unique_ptr<BaseAST>($1);
    ast->unary_exp_ast = unique_ptr<BaseAST>($3);

    ast->id = id++;
    $$ = ast;
  }
  | MulExp '/' UnaryExp{
    auto ast = new MulExpAST();
    ast->mul_exp_rule = 2;
    ast->mul_exp_ast = unique_ptr<BaseAST>($1);
    ast->unary_exp_ast = unique_ptr<BaseAST>($3);

    ast->id = id++;
    $$ = ast;
  }
  | MulExp '%' UnaryExp{
    auto ast = new MulExpAST();
    ast->mul_exp_rule = 3;
    ast->mul_exp_ast = unique_ptr<BaseAST>($1);
    ast->unary_exp_ast = unique_ptr<BaseAST>($3);

    ast->id = id++;
    $$ = ast;
  }
  ;

AddExp
  : MulExp {
    auto ast = new AddExpAST();
    ast->add_exp_rule = 0;
    ast->mul_exp_ast = unique_ptr<BaseAST>($1);
    ast->add_exp_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | AddExp '+' MulExp {
    auto ast = new AddExpAST();
    ast->add_exp_rule = 1;
    // last change jsa
    ast->add_exp_ast = unique_ptr<BaseAST>($1);
    ast->mul_exp_ast = unique_ptr<BaseAST>($3);
    
    ast->id = id++;
    $$ = ast;
  }
  | AddExp '-' MulExp {
    auto ast = new AddExpAST();
    ast->add_exp_rule = 2;
    // last change jsa
    ast->add_exp_ast = unique_ptr<BaseAST>($1);
    ast->mul_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
  }
  ;

RelExp
  : AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp_rule = 0;
    ast->add_exp_ast = unique_ptr<BaseAST>($1);
    ast->rel_exp_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | RelExp LT AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp_rule = 1;
    // last change jsa
    ast->rel_exp_ast = unique_ptr<BaseAST>($1);
    ast->add_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
  }
  | RelExp GT AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp_rule = 2;
    // last change jsa
    ast->rel_exp_ast = unique_ptr<BaseAST>($1);
    ast->add_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
  }
  | RelExp LE AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp_rule = 3;
    // last change jsa
    ast->rel_exp_ast = unique_ptr<BaseAST>($1);
    ast->add_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
  }
  | RelExp GE AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp_rule = 4;
    // last change jsa
    ast->rel_exp_ast = unique_ptr<BaseAST>($1);
    ast->add_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
  }
  ;

EqExp
  : RelExp {
    auto ast = new EqExpAST();
    ast->eq_rule = 0;
    ast->eq_exp_ast = nullptr;
    ast->rel_exp_ast = unique_ptr<BaseAST>($1);
    ast->id = id++;
    $$ = ast;
  }
  | EqExp EQ RelExp {
    auto ast = new EqExpAST();
    ast->eq_rule = 1;
    ast->eq_exp_ast = unique_ptr<BaseAST>($1);
    ast->rel_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
  }
  | EqExp NE RelExp {
    auto ast = new EqExpAST();
    ast->eq_rule = 2;
    ast->eq_exp_ast = unique_ptr<BaseAST>($1);
    ast->rel_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
  }
  ;

LAndExp
  : EqExp {
    auto ast = new LAndExpAST();
    ast->l_and_exp_rule = 0;
    ast->eq_exp_ast = unique_ptr<BaseAST>($1);
    ast->l_and_exp_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | LAndExp AND EqExp {
    auto ast = new LAndExpAST();
    ast->l_and_exp_rule = 1;
    ast->l_and_exp_ast = unique_ptr<BaseAST>($1);
    ast->eq_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
  }
  ;

LOrExp
  : LAndExp {
    auto ast = new LOrExpAST();
    ast->l_or_exp_rule = 0;
    ast->l_and_exp_ast = unique_ptr<BaseAST>($1);
    ast->l_or_exp_ast = nullptr;
    ast->id = id++;
    $$ = ast;
  }
  | LOrExp OR LAndExp {
    auto ast = new LOrExpAST();
    ast->l_or_exp_rule = 1;
    // jsa
    ast->l_or_exp_ast = unique_ptr<BaseAST>($1);
    ast->l_and_exp_ast = unique_ptr<BaseAST>($3);
    ast->id = id++;
    $$ = ast;
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

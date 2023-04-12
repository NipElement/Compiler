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

 
%token INT RETURN VOID CONST IF ELSE WHILE BREAK CONTINUE
%token <str_val> IDENT
%token <int_val> INT_CONST 


%type <ast_val> FuncDef FuncType Block Stmt Number Decl ConstDecl VarDecl 
BType ConstDef ConstDefList ConstExpList ConstInitVal ConstExp ConstInitValList 
VarDef VarDefList InitVal InitValList FuncFParams FuncFParamsList FuncFParam 
BlockItemList BlockItem LVal LOrExp ExpList1 PrimaryExp UnaryExp UnaryOp FuncRParams 
ExpList2 MulExp AddExp RelExp EqExp LAndExp 

%%


CompUnit
  : 
  | CompUnit Decl 
  | CompUnit FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    
    comp_unit->id = id++;
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;

Decl
  : ConstDecl 
  | VarDecl 
  ;

//ConstDecl     ::= "const" BType ConstDef {"," ConstDef} ";"
ConstDecl
  : CONST BType ConstDef ConstDefList ';'
  ;

ConstDefList
  : ',' ConstDef ConstDefList
  |
  ;

BType
  : INT 
  ;

ConstDef
  : IDENT ConstExpList '=' ConstInitVal 
  ;

ConstExpList
  : '[' ConstExp ']' ConstExpList
  |
  ;

//ConstInitVal  ::= ConstExp | "{" [ConstInitVal {"," ConstInitVal}] "}";
ConstInitVal
  : ConstExp 
  | '{'  '}'
  | '{' ConstInitVal ConstInitValList '}'
  ;

ConstInitValList
  : ',' ConstInitVal ConstInitValList
  |
  ;

VarDecl
  : IDENT BType VarDef VarDefList ';'
  ;

VarDefList
  : ',' VarDef VarDefList
  |
  ;

VarDef 
  : IDENT ConstExpList
  | IDENT ConstExpList '=' InitVal
  ;

InitVal
  : Exp
  | '{'  '}'
  | '{' InitVal InitValList '}'
  ;

InitValList
  : ',' InitVal InitValList
  |
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->id = id++;
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  | FuncType IDENT '(' FuncFParams ')' Block
  ;

FuncType
  : INT {
    auto ast = new FuncType();
    ast->id = id++;
    ast->type_ = FuncType::int_;
    $$ = ast;
  }
  | VOID{
    auto ast = new FuncType();
    ast->id = id++;
    ast->type_ = FuncType::void_;
    $$ = ast;
  }
  ;

FuncFParams
  : FuncFParams FuncFParamsList
  ;

FuncFParamsList
  : ',' FuncFParams FuncFParamsList
  |
  ;

FuncFParam
  : BType IDENT
  | BType IDENT '['  ']' ConstExpList
  ;

Block
  : '{' BlockItemList '}' {
  }
  ;

BlockItemList
  : BlockItem BlockItemList
  |
  ;

BlockItem
  : Decl
  | Stmt
  ;

Stmt
  : LVal '=' Exp ';'
  | ';'
  | Exp ';'
  | Block
  | IF '(' Exp ')' Stmt
  | IF '(' Exp ')' Stmt ELSE Stmt
  | WHILE '(' Exp ')' Stmt
  | BREAK ';'
  | CONTINUE ';'
  |RETURN ';'
  | RETURN Exp ';' {
    auto ast = new Stmt();
    ast->id = id++;
    ast->number = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

Exp
  : LOrExp
  ;

LVal
  : IDENT ExpList1
  ;

ExpList1
  : '[' Exp ']' ExpList1
  |
  ;

PrimaryExp    
  : '(' Exp ')'
  | LVal
  | Number{
    auto ast = new PrimaryExp();
    ast->type=number;
    ast->id++;
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

Number
  : INT_CONST {
    auto ast = new Number();
    ast->id = id++;
    ast->content = $1;
    $$ = ast;
  }
  ;

UnaryExp 
  : PrimaryExp
  | IDENT '(' ')'
  | IDENT '(' FuncRParams ')'
  | UnaryOp UnaryExp
  ;

UnaryOp
  : '+'
  | '-'
  | '!'
  ;

FuncRParams
  : Exp ExpList2
  ;

ExpList2
  : ',' Exp ExpList2
  |
  ;

MulExp
  : UnaryExp
  | MulExp '*' UnaryExp
  | MulExp '/' UnaryExp
  | MulExp '%' UnaryExp
  ;

AddExp
  : MulExp
  | AddExp '+' MulExp
  | AddExp '-' MulExp
  ;

RelExp
  : AddExp
  | RelExp '<' AddExp
  | RelExp '>' AddExp
  | RelExp '<=' AddExp
  | RelExp '>=' AddExp
  ;

EqExp
  : RelExp
  | EqExp '==' RelExp
  | EqExp '!=' RelExp
  ;

LAndExp
  : EqExp
  | LAndExp '&&' EqExp
  ;

LOrExp
  : LAndExp
  | LOrExp '||' LAndExp
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

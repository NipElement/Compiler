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


%type <ast_val> CompUnit FuncDef FuncType Block Stmt Number Decl ConstDecl VarDecl 
BType ConstDef ConstDefList ConstExpList ConstInitVal ConstExp ConstInitValList 
VarDef VarDefList InitVal InitValList FuncFParams FuncFParamsList FuncFParam 
BlockItemList BlockItem LVal LOrExp ExpList1 PrimaryExp UnaryExp UnaryOp FuncRParams 
ExpList2 MulExp AddExp RelExp EqExp LAndExp 

%%


CompUnit
  : CompUnit Decl {
      std::cout<<"CompUnit Decl"<<endl;
  }
  | CompUnit FuncDef {
    std::cout<<"CompUnit FuncDef"<<endl;
  }
  | {
    std::cout<<"Blank"<<endl;
  }
  ;

Decl
  : ConstDecl {
    std::cout<<"ConstDecl"<<endl;

  }
  | VarDecl {
    std::cout<<"VarDecl"<<endl;
    
  }
  ;

//ConstDecl     ::= "const" BType ConstDef {"," ConstDef} ";"
ConstDecl
  : CONST BType ConstDef ConstDefList ';'{
    std::cout<<"CONST BType ConstDef ConstDefList ';'"<<endl;
  }
  ;

ConstDefList
  :
  | ',' ConstDef ConstDefList{
    std::cout<<"',' ConstDef ConstDefList"<<endl;
  }
  
  ;

BType
  : INT {
    std::cout<<"INT"<<endl;
  }
  ;

ConstDef
  : IDENT ConstExpList '=' ConstInitVal{
    std::cout<<"IDENT ConstExpList '=' ConstInitVal"<<endl;
  } 
  ;

ConstExpList
  : 
  | ConstExpList '[' ConstExp ']' {
    std::cout<<" ConstExpList '[' ConstExp ']'"<<endl;
  }
  
  ;

//ConstInitVal  ::= ConstExp | "{" [ConstInitVal {"," ConstInitVal}] "}";
ConstInitVal
  : ConstExp {
    std::cout<<"ConstExp"<<endl;
  }
  | '{'  '}'{
    std::cout<<"'{'  '}'"<<endl;
  }
  | '{' ConstInitVal ConstInitValList '}'{
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
  : BType VarDef VarDefList ';'{
    std::cout<<"BType VarDef VarDefList ';'"<<endl;
  }
  ;

VarDefList
  : 
  | VarDefList ',' VarDef {
    std::cout<<"VarDefList ',' VarDef"<<endl;
  }
  ;

VarDef 
  : IDENT ConstExpList{
    std::cout<<"IDENT ConstExpList"<<endl;
  }
  | IDENT ConstExpList '=' InitVal{
    std::cout<<"IDENT ConstExpList '=' InitVal"<<endl;
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
  : FuncType IDENT '(' ')' Block {
    std::cout<<"FuncType IDENT '(' ')' Block"<<endl;
  }
  | FuncType IDENT '(' FuncFParams ')' Block {
    std::cout<<"FuncType IDENT '(' FuncFParams ')' Block"<<endl;
  }
  ;

FuncType
  : INT {
    std::cout<<"INT"<<endl;
  }
  | VOID{
    std::cout<<"VOID"<<endl;
  }
  ;

FuncFParams
  : FuncFParam FuncFParamsList{
    std::cout<<"FuncFParam FuncFParamsList"<<endl;
  }
  ;

FuncFParamsList
  : 
  | ',' FuncFParam FuncFParamsList{
    std::cout<<"',' FuncFParam FuncFParamsList"<<endl;
  }
  ;

FuncFParam
  : BType IDENT{
    std::cout<<"BType IDENT"<<endl;
  }
  | BType IDENT '['  ']' ConstExpList{
    std::cout<<"BType IDENT '['  ']' ConstExpList"<<endl;
  }
  ;

Block
  : '{' BlockItemList '}' {
    std::cout<<"'{' BlockItemList '}'"<<endl;
  }
  ;

BlockItemList
  : 
  | BlockItem BlockItemList{
    std::cout<<"BlockItem BlockItemList"<<endl;
  }  
  ;

BlockItem
  : Decl{
    std::cout<<"Decl"<<endl;
  }
  | Stmt{
    std::cout<<"Stmt"<<endl;
  }
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
  | RETURN ';'
  | RETURN Exp ';' 
  ;

Exp
  : LOrExp
  ;

LVal
  : IDENT ExpList1
  ;

ExpList1
  : 
  | '[' Exp ']' ExpList1  
  ;

PrimaryExp    
  : '(' Exp ')'
  | LVal
  | Number
  ;

Number
  : INT_CONST {
    std::cout<<"INT_CONST"<<endl;
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
  : 
  | ',' Exp ExpList2
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
  | RelExp LE AddExp
  | RelExp GE AddExp
  ;

EqExp
  : RelExp
  | EqExp EQ RelExp
  | EqExp NE RelExp
  ;

LAndExp
  : EqExp
  | LAndExp AND EqExp
  ;

LOrExp
  : LAndExp
  | LOrExp OR LAndExp
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

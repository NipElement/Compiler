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


%token INT RETURN VOID
%token <str_val> IDENT
%token <int_val> INT_CONST


%type <ast_val> FuncDef FuncType Block Stmt Number Decl

%%


CompUnit
  : FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    
    comp_unit->id = id++;
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  | Decl {

  }
  | CompUnit FuncDef {

  }
  | CompUnit Decl {

  } 
  ;

Decl
  : ConstDecl {

  }
  | VarDecl {

  }
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

Block
  : '{' Stmt '}' {
    auto ast = new Block();
    ast->id = id++;
    ast->stmt = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

Stmt
  : RETURN Number ';' {
    auto ast = new Stmt();
    ast->id = id++;
    ast->number = unique_ptr<BaseAST>($2);
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

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}

%{
#include <iostream>
#include <string>
#include <memory>
#include "ast.h"

extern int yylex();
extern void yyerror(const char* s);
extern int yylineno;

std::unique_ptr<ContractNode> root;

%}

%union {
    int intVal;
    char* strVal;
    bool boolVal;
    ExpressionNode* exprNode;
    StatementNode* stmtNode;
    FunctionNode* funcNode;
    ParameterNode* paramNode;
    ContractNode* contractNode;
    std::vector<StatementNode*>* stmtList;
    std::vector<FunctionNode*>* funcList;
    std::vector<ParameterNode*>* paramList;
    DataType dataType;
}

/* Terminal symbols */
%token <strVal> IDENTIFIER
%token <intVal> INTEGER_LITERAL
%token <strVal> ADDRESS_LITERAL
%token CONTRACT FUNCTION PUBLIC RETURNS IF ELSE RETURN
%token UINT INT BOOL ADDRESS_TYPE
%token TRUE FALSE
%token PLUS MINUS MULTIPLY DIVIDE MODULUS ASSIGN
%token EQ NE GT LT GE LE AND OR NOT
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA

/* Non-terminal symbols */
%type <contractNode> program
%type <funcList> function_list
%type <funcNode> function_def
%type <paramList> param_list param_list_opt
%type <paramNode> param
%type <dataType> type
%type <stmtList> block statement_list
%type <stmtNode> statement var_decl assignment if_statement return_statement
%type <exprNode> expression primary_expression

/* Operator precedence and associativity */
%left OR
%left AND
%left EQ NE
%left LT LE GT GE
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULUS
%right NOT
%right UMINUS

%%

program:
    CONTRACT IDENTIFIER LBRACE function_list RBRACE {
        $$ = new ContractNode($2, *$4);
        root = std::unique_ptr<ContractNode>($$);
        delete $4;
    }
    ;

function_list:
    /* empty */ {
        $$ = new std::vector<FunctionNode*>();
    }
    | function_list function_def {
        $1->push_back($2);
        $$ = $1;
    }
    ;

function_def:
    FUNCTION IDENTIFIER LPAREN param_list_opt RPAREN PUBLIC RETURNS LPAREN type RPAREN block {
        $$ = new FunctionNode($2, *$4, $9, *$11);
        delete $4;
        delete $11;
    }
    | type IDENTIFIER ASSIGN expression SEMICOLON {
        // Handle simple variable declarations as parameterless functions
        auto stmts = new std::vector<StatementNode*>();
        stmts->push_back(new ReturnStatementNode($4));
        $$ = new FunctionNode($2, std::vector<ParameterNode*>(), $1, *stmts);
        delete stmts;
    }
    ;

param_list_opt:
    /* empty */ {
        $$ = new std::vector<ParameterNode*>();
    }
    | param_list {
        $$ = $1;
    }
    ;

param_list:
    param {
        $$ = new std::vector<ParameterNode*>();
        $$->push_back($1);
    }
    | param_list COMMA param {
        $1->push_back($3);
        $$ = $1;
    }
    ;

param:
    type IDENTIFIER {
        $$ = new ParameterNode($1, $2);
    }
    ;

type:
    UINT { $$ = DataType::UINT; }
    | INT { $$ = DataType::INT; }
    | BOOL { $$ = DataType::BOOL; }
    | ADDRESS_TYPE { $$ = DataType::ADDRESS; }
    ;

block:
    LBRACE statement_list RBRACE {
        $$ = $2;
    }
    ;

statement_list:
    /* empty */ {
        $$ = new std::vector<StatementNode*>();
    }
    | statement_list statement {
        $1->push_back($2);
        $$ = $1;
    }
    ;

statement:
    var_decl { $$ = $1; }
    | assignment { $$ = $1; }
    | if_statement { $$ = $1; }
    | return_statement { $$ = $1; }
    | expression SEMICOLON {
        $$ = new ExpressionStatementNode($1);
    }
    ;

var_decl:
    type IDENTIFIER ASSIGN expression SEMICOLON {
        $$ = new VarDeclNode($1, $2, $4);
    }
    | type IDENTIFIER SEMICOLON {
        $$ = new VarDeclNode($1, $2, nullptr);
    }
    ;

assignment:
    IDENTIFIER ASSIGN expression SEMICOLON {
        $$ = new AssignmentNode($1, $3);
    }
    ;

if_statement:
    IF LPAREN expression RPAREN block {
        $$ = new IfStatementNode($3, *$5, std::vector<StatementNode*>());
        delete $5;
    }
    | IF LPAREN expression RPAREN block ELSE block {
        $$ = new IfStatementNode($3, *$5, *$7);
        delete $5;
        delete $7;
    }
    ;

return_statement:
    RETURN expression SEMICOLON {
        $$ = new ReturnStatementNode($2);
    }
    ;

expression:
    expression PLUS expression {
        $$ = new BinaryOpNode(BinaryOperator::PLUS, $1, $3);
    }
    | expression MINUS expression {
        $$ = new BinaryOpNode(BinaryOperator::MINUS, $1, $3);
    }
    | expression MULTIPLY expression {
        $$ = new BinaryOpNode(BinaryOperator::MULTIPLY, $1, $3);
    }
    | expression DIVIDE expression {
        $$ = new BinaryOpNode(BinaryOperator::DIVIDE, $1, $3);
    }
    | expression MODULUS expression {
        $$ = new BinaryOpNode(BinaryOperator::MODULUS, $1, $3);
    }
    | expression EQ expression {
        $$ = new BinaryOpNode(BinaryOperator::EQ, $1, $3);
    }
    | expression NE expression {
        $$ = new BinaryOpNode(BinaryOperator::NE, $1, $3);
    }
    | expression GT expression {
        $$ = new BinaryOpNode(BinaryOperator::GT, $1, $3);
    }
    | expression LT expression {
        $$ = new BinaryOpNode(BinaryOperator::LT, $1, $3);
    }
    | expression GE expression {
        $$ = new BinaryOpNode(BinaryOperator::GE, $1, $3);
    }
    | expression LE expression {
        $$ = new BinaryOpNode(BinaryOperator::LE, $1, $3);
    }
    | expression AND expression {
        $$ = new BinaryOpNode(BinaryOperator::AND, $1, $3);
    }
    | expression OR expression {
        $$ = new BinaryOpNode(BinaryOperator::OR, $1, $3);
    }
    | NOT expression {
        $$ = new UnaryOpNode(UnaryOperator::NOT, $2);
    }
    | MINUS expression %prec UMINUS {
        $$ = new UnaryOpNode(UnaryOperator::NEGATE, $2);
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }
    | primary_expression {
        $$ = $1;
    }
    ;

primary_expression:
    INTEGER_LITERAL {
        $$ = new IntegerLiteralNode($1);
    }
    | TRUE {
        $$ = new BoolLiteralNode(true);
    }
    | FALSE {
        $$ = new BoolLiteralNode(false);
    }
    | ADDRESS_LITERAL {
        $$ = new AddressLiteralNode($1);
    }
    | IDENTIFIER {
        $$ = new IdentifierNode($1);
    }
    ;

%%
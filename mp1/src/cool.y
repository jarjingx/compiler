/*
 *  cool.y
 *              Parser definition for the COOL language.
 *
 */
%{
#include <iostream>
#include "cool-tree.h"
#include "stringtab.h"
#include "utilities.h"

/* Add your own C declarations here */


/************************************************************************/
/*                DONT CHANGE ANYTHING IN THIS SECTION                  */

extern int yylex();           /* the entry point to the lexer  */
extern int curr_lineno;
extern char *curr_filename;
Program ast_root;            /* the result of the parse  */
Classes parse_results;       /* for use in semantic analysis */
int omerrs = 0;              /* number of errors in lexing and parsing */

/*
   The parser will always call the yyerror function when it encounters a parse
   error. The given yyerror implementation (see below) justs prints out the
   location in the file where the error was found. You should not change the
   error message of yyerror, since it will be used for grading puproses.
*/
void yyerror(const char *s);

/*
   The VERBOSE_ERRORS flag can be used in order to provide more detailed error
   messages. You can use the flag like this:

     if (VERBOSE_ERRORS)
       fprintf(stderr, "semicolon missing from end of declaration of class\n");

   By default the flag is set to 0. If you want to set it to 1 and see your
   verbose error messages, invoke your parser with the -v flag.

   You should try to provide accurate and detailed error messages. A small part
   of your grade will be for good quality error messages.
*/
extern int VERBOSE_ERRORS;

%}

/* A union of all the types that can be the result of parsing actions. */
%union {
  Boolean boolean;
  Symbol symbol;
  Program program;
  Class_ class_;
  Classes classes;
  Feature feature;
  Features features;
  Formal formal;
  Formals formals;
  Case case_;
  Cases cases;
  Expression expression;
  Expressions expressions;
  char *error_msg;
}

/* 
   Declare the terminals; a few have types for associated lexemes.
   The token ERROR is never used in the parser; thus, it is a parse
   error when the lexer returns it.

   The integer following token declaration is the numeric constant used
   to represent that token internally.  Typically, Bison generates these
   on its own, but we give explicit numbers to prevent version parity
   problems (bison 1.25 and earlier start at 258, later versions -- at
   257)
*/
%token CLASS 258 ELSE 259 FI 260 IF 261 IN 262 
%token INHERITS 263 LET 264 LOOP 265 POOL 266 THEN 267 WHILE 268
%token CASE 269 ESAC 270 OF 271 DARROW 272 NEW 273 ISVOID 274
%token <symbol>  STR_CONST 275 INT_CONST 276 
%token <boolean> BOOL_CONST 277
%token <symbol>  TYPEID 278 OBJECTID 279 
%token ASSIGN 280 NOT 281 LE 282 ERROR 283

/*  DON'T CHANGE ANYTHING ABOVE THIS LINE, OR YOUR PARSER WONT WORK       */
/**************************************************************************/
 
   /* Complete the nonterminal list below, giving a type for the semantic
      value of each non terminal. (See section 3.6 in the bison 
      documentation for details). */

/* Declare types for the grammar's non-terminals. */
%type <program> program

%type <classes> class_list
%type <class_> class

%type <features> feature_list
%type <feature> feature

%type <formals> formal_list
%type <formal> formal

%type <expression> expr
%type <expressions> expr_list
%type <expressions> expr_list_comma

%type <case_> case
%type <cases> case_list

%type <expression> let

/* Precedence declarations go here. */
%right ASSIGN
%right NOT
%nonassoc LE '<' '='
%left '+' '-'
%left '*' '/'
%right ISVOID
%right '~'
%right '@'
%right '.'

%%
/* Save the root of the abstract syntax tree in a global variable. */
program : class_list { ast_root = program($1); }
        ;

/* If no parent is specified, the class inherits from the Object class. */

//===============================================================================================
// 类的定义：其中 C1 是为了能在发现错误后及时执行 yyerrok 而引入的标记非终结符
//===============================================================================================
class  	: CLASS TYPEID '{' feature_list '}'
                { $$ = class_($2,idtable.add_string("Object"),$4, stringtable.add_string(curr_filename)); }
        | CLASS TYPEID INHERITS TYPEID '{' feature_list '}'
                { $$ = class_($2,$4,$6,stringtable.add_string(curr_filename)); }
        | CLASS TYPEID INHERITS TYPEID '{' '}'
                { $$ = class_($2,$4, nil_Features(), stringtable.add_string(curr_filename)); }
	| CLASS C1 feature_list '}'
		{ yyerrok; }
        ;

C1	: error '{'
		{ yyerrok; }
	;

class_list
        : class ';'
                { $$ = single_Classes($1); }
        | class_list class ';'
                { $$ = append_Classes($1,single_Classes($2)); }
	| error ';'
		{ yyerrok; }
	| class_list error ';'
		{ yyerrok; }
        ;
//===============================================================================================



//===============================================================================================
// 参数列表的定义
//===============================================================================================
formal	: OBJECTID ':' TYPEID
		{ $$ = formal($1, $3); }
	| error ','
		{ yyerrok; yychar = ','; }
	| error ')'
		{ yyerrok; yychar = ')'; }
	;

formal_list
	: formal
		{ $$ = single_Formals( $1 ); }
	| formal_list ',' formal
		{ $$ = append_Formals( $1, single_Formals( $3 ) ); }
	|
		{ $$ = nil_Formals(); }	
	;
//===============================================================================================



//===============================================================================================
// 特性的定义：其中 Fe1、Fe2、Fe3 是为了能在发现错误后及时执行 yyerrok 而引入的标记非终结符
//===============================================================================================
feature	: OBJECTID '(' formal_list ')' ':' TYPEID '{' expr '}'
		{ $$ = method( $1, $3, $6, $8 ); printf( "1\n" ); }
	| OBJECTID '(' formal_list ')' ':' TYPEID '{' error '}'
		{ yyerrok; }
	| Fe1 formal_list ')' ':' TYPEID '{' expr '}'
		{ yyerrok; }
	| Fe1 formal_list ')' ':' TYPEID '{' error '}'
		{ yyerrok; }
	| OBJECTID '(' formal_list ')' ':' Fe2 expr '}'
		{ yyerrok; }
	| OBJECTID '(' formal_list ')' ':' Fe2 error '}'
		{ yyerrok; }
	| Fe1 formal_list ')' ':' Fe2 expr '}'
		{ yyerrok; }
	| Fe1 formal_list ')' ':' Fe2 error '}'
		{ yyerrok; }
	| OBJECTID ':' TYPEID ASSIGN expr
		{ $$ = attr( $1, $3, $5 ); }
	| OBJECTID ':' TYPEID ASSIGN error ';'
		{ yyerrok; yychar = ';'; }
	| OBJECTID ':' TYPEID
		{ $$ = attr( $1, $3, no_expr() ); }
	| Fe3 expr
		{ yyerrok; }
	| Fe3 error ';'
		{ yyerrok; yychar = ';'; }
	| error ';'
		{ yyerrok; yychar = ';'; }
	;

Fe1	: error '('
		{ yyerrok; }
	;

Fe2	: error '{'
		{ yyerrok; }
	;

Fe3	: error ASSIGN
		{ yyerrok; }
	;

feature_list
	: feature ';'
		{ $$ = single_Features( $1 ); }
	| feature_list feature ';'
		{ $$ = append_Features( $1, single_Features( $2 ) ); }
	| error ';'
		{ yyerrok; }
	| feature_list error ';'
		{ yyerrok; }
	|
		{ $$ = nil_Features(); }
	;
//===============================================================================================



//===============================================================================================
// case 语句的定义：其中 Ca1、Ca2 是为了能在发现错误后及时执行 yyerrok 而引入的标记非终结符
//===============================================================================================
case	: OBJECTID ':' TYPEID DARROW expr ';'
		{ $$ = branch( $1, $3, $5 ); }
	| OBJECTID ':' TYPEID DARROW error ';'
		{ yyerrok; }
	| Ca2 expr ';'
		{ yyerrok; }	
	| Ca2 error ';'
		{ yyerrok; }	
	| error ';'
		{ yyerrok; }
	;

Ca1	: error OF
		{ yyerrok; }
	;
	
Ca2	: error DARROW
		{ yyerrok; }
	;

case_list
	: case 
		{ $$ = single_Cases( $1 ); }
	| case_list case
		{ $$ = append_Cases( $1, single_Cases( $2 ) ); }
	;
//===============================================================================================



//===============================================================================================
// let 语句的定义
//===============================================================================================
let	: OBJECTID ':' TYPEID IN expr
		{ $$ = let( $1, $3, no_expr(), $5 ); }
	| OBJECTID ':' TYPEID ASSIGN expr IN expr
		{ $$ = let( $1, $3, $5, $7 ); }
	| OBJECTID ':' TYPEID ',' let
		{ $$ = let( $1, $3, no_expr(), $5); }
	| OBJECTID ':' TYPEID ASSIGN expr ',' let
		{ $$ = let( $1, $3, $5, $7 ); }
	| error IN expr
		{ yyerrok; }
	;
//===============================================================================================



//===============================================================================================
// 表达式的定义
//===============================================================================================
expr	// 处理赋值语句	
	: OBJECTID ASSIGN expr
		{ $$ = assign( $1, $3 ); }

	// 处理第一种形式的方法调用
	| expr '@' TYPEID '.' OBJECTID '(' expr_list_comma ')'
		{ $$ = static_dispatch( $1, $3, $5, $7 ); }
	| expr '@' M1 OBJECTID '(' expr_list_comma ')'
		{ yyerrok; }
	| expr '@' TYPEID '.' M2 expr_list_comma ')'
		{ yyerrok; }
	| expr '@' M1 M2 expr_list_comma ')'
		{ yyerrok; }

	// 处理第二种形式的方法调用
	| expr '.' OBJECTID '(' expr_list_comma ')'
		{ $$ = dispatch( $1, $3, $5 ); }
	| expr '.' M2 expr_list_comma ')'
		{ yyerrok; }

	// 处理第三种形式的方法调用
	| OBJECTID '(' expr_list_comma ')'
		{ $$ = dispatch( object( idtable.add_string( "self" ) ), $1, $3 ); }
	| M2 expr_list_comma ')'
		{ yyerrok; }

	// 处理条件语句
	| IF expr THEN expr ELSE expr FI
		{ $$ = cond( $2, $4, $6 ); }
	/*| I1 THEN expr ELSE expr FI
		{ yyerrok; }
	| IF expr I2 ELSE expr FI
		{ yyerrok; }
	| IF expr THEN expr I3
		{ yyerrok; }
	| I1 I2 ELSE expr FI
		{ yyerrok; }
	| I1 THEN expr I3
		{ yyerrok; }
	| IF expr I2 I3
		{ yyerrok; }
	| I1 I2 I3
		{ yyerrok; }
	| IF error THEN error ELSE error FI
		{ yyerrok; }*/

	// 处理循环语句
	| WHILE expr LOOP expr POOL
		{ $$ = loop( $2, $4 ); }
	/*| WHILE W1 expr POOL
		{ yyerrok; }
	| WHILE expr LOOP W2
		{ yyerrok; }
	| WHILE W1 W2
		{ yyerrok; }*/
	| WHILE error POOL
		{ yyerrok; }
	

	// 处理语句块
	| '{' expr_list '}'
		{ $$ = block( $2 ); }

	// 处理 let 语句
	| LET let
		{ $$ = $2; }

	// 处理 case 语句
	| CASE expr OF case_list ESAC
		{ $$ = typcase( $2, $4 ); }
	| CASE Ca1 case_list ESAC
		{ yyerrok; }

	// 处理运算
	| NEW TYPEID
		{ $$ = new_( $2 ); }
	| ISVOID expr
		{ $$ = isvoid( $2 ); }
	| expr '+' expr
		{ $$ = plus( $1, $3 ); }
	| expr '-' expr
		{ $$ = sub( $1, $3 ); }
	| expr '*' expr
		{ $$ = mul( $1, $3 ); }
	| expr '/' expr
		{ $$ = divide( $1, $3 ); }
	| '~' expr
		{ $$ = neg( $2 ); }
	| expr '<' expr
		{ $$ = lt( $1, $3 ); }
	| expr LE expr
		{ $$ = leq( $1, $3 ); }
	| expr '=' expr
		{ $$ = eq( $1, $3 ); }
	| NOT expr
		{ $$ = comp( $2 ); }
	| '(' expr ')'
		{ $$ = $2; } 
	
	// 处理表达式元
	| OBJECTID
		{ $$ = object( $1 ); }
	| INT_CONST
		{ $$ = int_const( $1 ); }
	| STR_CONST
		{ $$ = string_const( $1 ); }
	| BOOL_CONST
		{ $$ = bool_const( $1 ); }

	// 处理错误
	| error '@'
		{ yyerrok; yychar = '@'; }
	| error '.'
		{ yyerrok; yychar = '.'; }
	;

M1	: error '.'
		{ yyerrok; }
	;

M2	: error '('
		{ yyerrok; }
	;
/*
I1	: IF error THEN
		{ yyerrok; yyclearin; yychar = THEN; printf( "I1\n" ); }
	;

I2	: THEN error ELSE
		{ yyerrok; yyclearin; yychar = ELSE; printf( "I2\n" );}
	;

I3	: ELSE error FI
		{ yyerrok; printf( "I3\n" ); }
	;

W1	: error LOOP
		{ yyerrok; }
	;

W2	: error POOL
		{ yyerrok; }
	;
*/
expr_list
	: expr ';'
		{ $$ = single_Expressions( $1 ); }
	| expr_list expr ';'
		{ $$ = append_Expressions( $1, single_Expressions( $2 ) ); }
	| error ';'
		{ yyerrok; }
	| expr_list error ';'
		{ yyerrok; }
	;

expr_list_comma
	: expr
		{ $$ = single_Expressions($1); }
	| expr_list_comma ',' expr
		{ $$ = append_Expressions( $1, single_Expressions( $3 ) ); }
	|
		{ $$ = nil_Expressions( ); }
	| error ','
		{ yyerrok; yychar = ','; }
	| error ')'
		{ yyerrok; yychar = ')'; }
	| expr_list_comma ',' error ','
		{ yyerrok; yychar = ','; }
	| expr_list_comma ',' error ')'
		{ yyerrok; yychar = ')'; }
	;
//===============================================================================================



/* end of grammar */
%%

/* This function is called automatically when Bison detects a parse error. */
void yyerror(const char *s)
{
  cerr << "\"" << curr_filename << "\", line " << curr_lineno << ": " \
    << s << " at or near ";
  print_cool_token(yychar);
  cerr << endl;
  omerrs++;

  if(omerrs>20) {
      if (VERBOSE_ERRORS)
         fprintf(stderr, "More than 20 errors\n");
      exit(1);
  }
}


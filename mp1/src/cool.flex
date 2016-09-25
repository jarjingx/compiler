/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <string.h>
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
  if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
    YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;

extern YYSTYPE cool_yylval;

int	flag;
/*
 *  Add Your own definitions here
 */

%}

%option noyywrap

/*
 * Define names for regular expressions here.
 */

delim		[ \b\t\f]
digit       	[0-9]
integer		{digit}+
letter_lower	[a-z]
letter_upper	[A-Z]
letter		[A-Za-z]
typeid		{letter_upper}({letter}|{digit}|"_")*
objectid	{letter_lower}({letter}|{digit}|"_")*

%x		com str

%%
{delim}
\n		curr_lineno++;

(?i:class)	return( CLASS );
(?i:else)	return( ELSE );
(?i:fi)		return( FI );
(?i:if)		return( IF );
(?i:in)		return( IN );
(?i:inherits)	return( INHERITS );
(?i:isvoid)	return( ISVOID );
(?i:let)	return( LET );
(?i:loop)	return( LOOP );
(?i:pool)	return( POOL );
(?i:then)	return( THEN );
(?i:while)	return( WHILE );
(?i:case)	return( CASE );
(?i:esac)	return( ESAC );
(?i:new)	return( NEW );
(?i:of)		return( OF );
(?i:not)	return( NOT );
"<="		return( LE );
"=>"		return( DARROW );
"<-"		return( ASSIGN );
"+"		return( '+' );
"-"		return( '-' );
"*"		return( '*' );
"/"		return( '/' );
"~"		return( '~' );
"="		return( '=' );
"<"		return( '<' );
"("		return( '(' );
")"		return( ')' );
"@"		return( '@' );
"{"		return( '{' );
"}"		return( '}' );
";"		return( ';' );
","		return( ',' );
":"		return( ':' );
"."		return( '.' );

		/* deal with boolean */
t(?i:rue)	{
			yylval.boolean = true;
			return( BOOL_CONST );
		}
f(?i:alse)	{
			yylval.boolean = false;
			return( BOOL_CONST );
		}

		/* deal with identification */
{typeid}	{
			yylval.symbol = idtable.add_string( yytext );
			return( TYPEID );
		}
{objectid}	{
			yylval.symbol = idtable.add_string( yytext );
			return( OBJECTID );
		}

		/* deal with integer */
{integer}	{
			yylval.symbol = inttable.add_string( yytext );
			return( INT_CONST );
		}

		/* deal with unmatched sign */
"*)"		{ 	yylval.error_msg = "Unmatched *)";
			return( ERROR );
		}
		
		/* deal with comment */
"(*"		BEGIN( com );
<com>[^*\n]*	
<com>"*"+[^*)\n]*
<com>\n		curr_lineno++;
<com><<EOF>>	{
			yylval.error_msg = "EOF in comment";
			BEGIN( INITIAL );
			return( ERROR );
		}
<com>"*"+")"	BEGIN( INITIAL );

		/* deal with string */
\"		{
			string_buf_ptr = string_buf;
			BEGIN( str );
			flag = 0;		
		}
<str>\"		{
			BEGIN( INITIAL );
			*string_buf_ptr = '\0';
			if( yyleng > 1025 )
			{
				yylval.error_msg = "String constant too long";
				return( ERROR );
			}
			if( flag )
			{
				yylval.error_msg = "String contains null character";
				return( ERROR );
			}
			yylval.symbol = stringtable.add_string( string_buf );
			return( STR_CONST );
		}
<str>\n		{
			curr_lineno++;
			yylval.error_msg = "Unterminated string constant";
			BEGIN( INITIAL );
			return( ERROR );
		}
<str><<EOF>>	{
			yylval.error_msg = "EOF in string constant";
			BEGIN( INITIAL );
			return( ERROR );
		}
<str>\0		if( flag == 0 ) flag = 1;
<str>\\n	*string_buf_ptr++ = '\n';
<str>\\t	*string_buf_ptr++ = '\t';
<str>\\r	*string_buf_ptr++ = '\r';
<str>\\b	*string_buf_ptr++ = '\b';
<str>\\f	*string_buf_ptr++ = '\f';
<str>\\(.|\n)	*string_buf_ptr++ = yytext[1];
<str>[^\\\n\"]+	{
			char *yptr = yytext;
			while( *yptr )
				*string_buf_ptr++ = *yptr++;
		}

		/* deal with invalid character */
.		{	yylval.error_msg = yytext;
			return( ERROR );
		}

 /*
  * Define regular expressions for the tokens of COOL here. Make sure, you
  * handle correctly special cases, like:
  *   - Nested comments
  *   - String constants: They use C like systax and can contain escape
  *     sequences. Escape sequence \c is accepted for all characters c. Except
  *     for \n \t \b \f, the result is c.
  *   - Keywords: They are case-insensitive except for the values true and
  *     false, which must begin with a lower-case letter.
  *   - Multiple-character operators (like <-): The scanner should produce a
  *     single token for every such operator.
  *   - Line counting: You should keep the global variable curr_lineno updated
  *     with the correct line number
  */

%%

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			([Ee][+-]?{D}+)
P                       ([Pp][+-]?{D}+)
FS			(f|F|l|L)
IS                      ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))

%{
#include <stdio.h>
#include "../../util/uhdm_parse.h"
#include "uhdm.tab.h"

int column = 0;

static int oldcol;

const char *from_code(int code)
{
static char codstr[30];
switch(code)
    {
    case ID : return yytext;
    case CONSTANT : return "CONSTANT";
    case STRING_LITERAL : return "STRING_LITERAL";
    case BACK : return "BACK";
    case START : return "START";
    case END : return "END";
    case NL : return "NL";
    case Restored : return "Restored";
    case parent : return "parent";
    case INT : return "INT";
    case always : return "always";
    case assignment : return "assignment";
    case begin : return "begin";
    case clk : return "clk";
    case constant : return "constant";
    case design : return "design";
    case event_control : return "event_control";
    case module : return "module";
    case obj : return "obj";
    case out : return "out";
    case package : return "package";
    case process : return "process";
    case ref_obj : return "ref_obj";
    case uhdmallModules : return "uhdmallModules";
    case uhdmallPackages : return "uhdmallPackages";
    case vpiAlwaysType : return "vpiAlwaysType";
    case vpiCondition : return "vpiCondition";
    case vpiDefName : return "vpiDefName";
    case vpiFullName : return "vpiFullName";
    case vpiLhs : return "vpiLhs";
    case vpiName : return "vpiName";
    case vpiProcess : return "vpiProcess";
    case vpiRhs : return "vpiRhs";
    case vpiStmt : return "vpiStmt";
    case vpiTopModule : return "vpiTopModule";
    }
sprintf(codstr, "\"%c\"", code);
return codstr;
}

int logging(int code)
{
old_code = code;
if (verbose_mode) puts(from_code(code));
return code;
}

%}

%%
"/*"			{ comment(); }
"//"[^\n]*              { /* consume //-comment */ }

"INT"       { count(); return ( logging (INT) ); }
"STRING:"[^\n]*    { count(); return ( logging (STRING) ); }
"always"    { count(); return ( logging (always) ); }
"assignment"    { count(); return ( logging (assignment) ); }
"begin"    { count(); return ( logging (begin) ); }
"constant"    { count(); return ( logging (constant) ); }
"design"    { count(); return ( logging (design) ); }
"delay_control"    { count(); return ( logging (delay_control) ); }
"event_control"    { count(); return ( logging (event_control) ); }
"function"    { count(); return ( logging (function) ); }
"initial"    { count(); return ( logging (initial) ); }
"module"    { count(); return ( logging (module) ); }
"package"    { count(); return ( logging (package) ); }
"parent"    { count(); return ( logging (parent) ); }
"process"    { count(); return ( logging (process) ); }
"ref_obj"    { count(); return ( logging (ref_obj) ); }
"uhdmallModules"    { count(); return ( logging (uhdmallModules) ); }
"uhdmallPackages"    { count(); return ( logging (uhdmallPackages) ); }
"vpiAlwaysType"    { count(); return ( logging (vpiAlwaysType) ); }
"vpiCondition"    { count(); return ( logging (vpiCondition) ); }
"vpiDefName"    { count(); return ( logging (vpiDefName) ); }
"vpiFullName"    { count(); return ( logging (vpiFullName) ); }
"vpiInstance"    { count(); return ( logging (vpiInstance) ); }
"vpiLhs"    { count(); return ( logging (vpiLhs) ); }
"vpiModule"    { count(); return ( logging (vpiModule) ); }
"vpiName"    { count(); return ( logging (vpiName) ); }
"vpiProcess"    { count(); return ( logging (vpiProcess) ); }
"vpiRhs"    { count(); return ( logging (vpiRhs) ); }
"vpiSize"    { count(); return ( logging (vpiSize) ); }
"vpiStmt"    { count(); return ( logging (vpiStmt) ); }
"vpiTaskFunc"    { count(); return ( logging (vpiTaskFunc) ); }
"vpiTopModule"    { count(); return ( logging (vpiTopModule) ); }

{L}({L}|{D})*		{ count(); return( logging ( ID) ); }

0[xX]{H}+{IS}?		{ count(); return( logging ( CONSTANT) ); }
0[0-7]*{IS}?		{ count(); return( logging ( CONSTANT) ); }
[1-9]{D}*{IS}?		{ count(); return( logging ( CONSTANT) ); }
L?'(\\.|[^\\'\n])+'	{ count(); return( logging ( CONSTANT) ); }

{D}+{E}{FS}?		{ count(); return( logging ( CONSTANT) ); }
{D}*"."{D}+{E}?{FS}?	{ count(); return( logging ( CONSTANT) ); }
{D}+"."{D}*{E}?{FS}?	{ count(); return( logging ( CONSTANT) ); }
0[xX]{H}+{P}{FS}?	{ count(); return( logging ( CONSTANT) ); }
0[xX]{H}*"."{H}+{P}?{FS}?     { count(); return( logging ( CONSTANT) ); }
0[xX]{H}+"."{H}*{P}?{FS}?     { count(); return( logging ( CONSTANT) ); }


L?\"(\\.|[^\\"\n])*\"	{ count(); return( logging ( STRING_LITERAL) ); }

";"			{ count(); return( logging ( ';') ); }
"\\_"		        { count(); return( logging ( BACK) ); }
("}"|"%>")		{ count(); return( logging ( '}') ); }
","			{ count(); return( logging ( ',') ); }
":"			{ count(); return( logging ( ':') ); }
"="			{ count(); return( logging ( '=') ); }
"("			{ count(); return( logging ( '(') ); }
")"			{ count(); return( logging ( ')') ); }
("["|"<:")		{ count(); return( logging ( '[') ); }
("]"|":>")		{ count(); return( logging ( ']') ); }
"."			{ count(); return( logging ( '.') ); }
"&"			{ count(); return( logging ( '&') ); }
"!"			{ count(); return( logging ( '!') ); }
"~"			{ count(); return( logging ( '~') ); }
"-"			{ count(); return( logging ( '-') ); }
"+"			{ count(); return( logging ( '+') ); }
"*"			{ count(); return( logging ( '*') ); }
"/"			{ count(); return( logging ( '/') ); }
"%"			{ count(); return( logging ( '%') ); }
"<"			{ count(); return( logging ( '<') ); }
">"			{ count(); return( logging ( '>') ); }
"^"			{ count(); return( logging ( '^') ); }
"|"			{ count(); return( logging ( '|') ); }
"?"			{ count(); return( logging ( '?') ); }
"#"			{ count(); return( logging ( '#') ); }

"\n"[" "]*		{ int col, oldcol2 = oldcol; count(); col = strlen(yytext); oldcol = col; if (verbose_mode > 1) printf("\n%d:%d\n", oldcol2, col);
                          return logging ( oldcol2 == col ? NL : oldcol2 < col ? START : END ); }
.			{  }

%%

int yywrap(void)
{
	return 1;
}


void comment(void)
{
	char c, prev = 0;
  
	while ((c = input()) != 0)      /* (EOF maps to 0) */
	{
		if (c == '/' && prev == '*')
			return;
		prev = c;
	}
	error("unterminated comment");
}

void count(void)
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			column = 0;
		else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;

	if (echo_mode) ECHO;
}

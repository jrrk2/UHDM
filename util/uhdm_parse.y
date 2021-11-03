%token <str> ID
%token CONSTANT STRING_LITERAL BACK START END NL
%token  Restored
%token  parent
%token INT
%token <str> STRING
%token always
%token assignment
%token begin
%token clk
%token constant
%token design
%token delay_control
%token event_control
%token function
%token initial
%token module
%token obj
%token out
%token package
%token process
%token process.uhdm
%token ref_obj
%token uhdmallModules
%token uhdmallPackages
%token vpiAlwaysType
%token vpiCondition
%token vpiDefName
%token vpiFullName
%token vpiInstance
%token vpiLhs
%token vpiModule
%token vpiName
%token vpiProcess
%token vpiRhs
%token vpiSize
%token vpiStmt
%token vpiTaskFunc
%token vpiTopModule

%start start

%{
#include <string.h>
#include "../../util/uhdm_parse.h"

static vpi_obj *vtmp;

%}

%union {
  int constant;
  char *str;
  void *ptr;
  struct vpi_obj *vpi;
  struct top_object *top;
  struct ref_object *ref;
  struct event_object *event;
  struct delay_object *delay;
  struct module_object *mod;
  struct cst_object *cst;
  struct asgn_object *asgn;
  struct always_object *alw;
  struct initial_object *init;
  struct bgn_object *bgn;
  struct pkg_object *pkg;
  struct func_object *func;
  struct attr_list *lst;
  }

%type <top> start
%type <str> id pth string
%type <vpi> vpi
%type <ptr> back parent
%type <lst> attr attr_lst
%type <constant> CONSTANT
%type <mod> modobj
%type <event> evobj
%type <delay> dlyobj
%type <ref> refobj
%type <cst> cstobj
%type <bgn> bgnobj
%type <pkg> pkgobj
%type <func> funcobj
%type <alw> alwobj
%type <init> initobj
%type <asgn> asgnobj
%%

start:  file ':' id design id '-' id ':' START vpi attr_lst { top.typ = TOP_OBJ; top.vtop = $10; top.attr = $11; $$ = &top; if (verbose_parse) printf("top=%p, %p\n", $10, $11); }
	;

file:     { }
        | file '.'
	| file id
	| file '-'

vpi:      vpiDefName ':' id { $$ = VPI(DEF_NAME, (void *)$3, 0); }
	| vpiName ':' id { $$ = VPI(NAME, (void *)$3, 0); }
	| vpiFullName ':' pth { $$ = VPI(FULL_NAME, (void *)$3, 0); }
	| vpiTopModule ':' CONSTANT { $$ = VPI(TOP_MOD, NULL, $3); }
	| vpiProcess ':' NL BACK back { $$ = VPI(PROC, $5, 0); if (verbose_parse) printf("proc: %p, back = %p\n", $$, $5); }
	| uhdmallPackages ':' NL BACK back {  $$ = VPI(ALL_PACK, $5, 0); if (verbose_parse) printf("allpack: %p, back = %p\n", $$, $5); }
	| uhdmallModules ':' NL BACK back { $$ = VPI(ALL_MOD, $5, 0); if (verbose_parse) printf("allmod: %p, back = %p\n", $$, $5); }
	| vpiModule ':' NL BACK back { $$ = VPI(ONE_MOD, $5, 0); if (verbose_parse) printf("mod: %p, back = %p\n", $$, $5); }
	| vpiInstance ':' NL BACK back { $$ = VPI(INST, $5, 0); if (verbose_parse) printf("inst: %p, back = %p\n", $$, $5); }
	| design ':' '(' id ')' { $$ = VPI(DESIGN, (void *)$4, 0); if (verbose_parse) printf("design: %p, id = %s\n", $$, $4); }
	| vpiAlwaysType ':' CONSTANT { $$ = VPI(ALWAYS, NULL, $3); if (verbose_parse) printf("always_type: %p\n", $$); }
	| vpiStmt ':' NL BACK back { $$ = VPI(STMT, $5, 0); if (verbose_parse) printf("stmt: %p, back = %p\n", $$, $5); }
	| vpiTaskFunc ':' NL BACK back { $$ = VPI(TASKF, $5, 0); }
	| vpiCondition ':' NL BACK back { $$ = VPI(COND, $5, 0); }
	| vpiLhs ':' NL BACK back { $$ = VPI(LHS, $5, 0); }
	| vpiRhs ':' NL BACK back { $$ = VPI(RHS, $5, 0); }
	| vpiSize ':' CONSTANT { $$ = VPI(SIZ, NULL, $3); }
	| INT ':' CONSTANT { $$ = VPI(CST, NULL, $3); }
	| '#' CONSTANT { $$ = VPI(HASH, NULL, $2); }
	| string { $$ = VPI(STRCST, (void *)$1, 0); }

attr:     START attr_lst opt_end { int cnt; $$ = rev_lst ($2, &cnt, verbose_parse); if (verbose_parse) printf("attr: %p, rev = %p (len=%d)\n", $2, $$, cnt); }
        | END { $$ = NULL; }

attr_lst: { $$ = NULL; }
	| NL { $$ = NULL; }
	| START { $$ = NULL; }
	| attr_lst '|' vpi opt_nl { attr_list *lst = malloc(sizeof(attr_list)); lst->nxt = $1; lst->itm = $3; $$ = lst; }
;

opt_nl:       { }
 	| NL  { }

opt_end:      { }
	| END { }

back:     modobj { $$ = $1; }
	| evobj { $$ = $1;  }
	| dlyobj { $$ = $1;  }
	| refobj { $$ = $1; }
	| cstobj { $$ = $1; }
	| asgnobj { $$ = $1; }
	| bgnobj { $$ = $1; }
	| pkgobj { $$ = $1; }
	| funcobj { $$ = $1; }
	| alwobj { $$ = $1; }
	| initobj { $$ = $1; }
;

pkgobj:	  package ':' id '(' id ')' attr { pkg_object *ptr = malloc(sizeof(pkg_object)); ptr->typ=PKG_OBJ; ptr->id1 = $3; ptr->id2 = $5; ptr->attr = $7; $$ = ptr; }

funcobj:  function ':' '(' id ')' attr { func_object *ptr = malloc(sizeof(func_object)); ptr->typ=FUNC_OBJ; ptr->id = $4; ptr->attr = $6; $$ = ptr; }

bgnobj:	  begin ':' attr { bgn_object *ptr = malloc(sizeof(bgn_object)); ptr->typ=BGN_OBJ; ptr->attr = $3; $$ = ptr; }

cstobj:	  constant ':' attr { cst_object *ptr = malloc(sizeof(cst_object)); ptr->typ=CST_OBJ; ptr->attr = $3; $$ = ptr; }

alwobj:   always ':' attr { always_object *ptr = malloc(sizeof(always_object)); ptr->typ=ALWYS_OBJ; ptr->attr = $3; $$ = ptr; }

initobj:  initial ':' attr { initial_object *ptr = malloc(sizeof(initial_object)); ptr->typ=INIT_OBJ; ptr->attr = $3; $$ = ptr; }

asgnobj:  assignment ':' attr { asgn_object *ptr = malloc(sizeof(asgn_object)); ptr->typ=ASGN_OBJ; ptr->attr = $3; $$ = ptr; }

refobj:	  ref_obj ':' '(' id ')' attr { ref_object *ptr = malloc(sizeof(ref_object)); ptr->typ=REF_OBJ; ptr->id = $4; ptr->attr = $6; $$ = ptr; }

evobj:	  event_control ':' attr { event_object *ptr = malloc(sizeof(event_object)); ptr->typ=EV_OBJ; ptr->attr = $3; $$ = ptr; }
dlyobj:	  delay_control ':' attr { delay_object *ptr = malloc(sizeof(delay_object)); ptr->typ=DLY_OBJ; ptr->attr = $3; $$ = ptr; }

modobj:	  module ':' id '(' pth ')' id '.' id ':' CONSTANT ':' ',' parent ':' pth attr { module_object *ptr = malloc(sizeof(module_object)); 
            ptr->typ=MOD_OBJ; ptr->id1 = $3; ptr->id2 = $5; ptr->id3 = $7; ptr->id4 = $9; ptr->lin = $11; ptr->par = $14; ptr->id5 = $16; ptr->attr = $17; $$ = ptr; }

pth:      id { $$=$1; }
        | pth '.' id { char *ret; asprintf(&ret, "%s.%s", $1, $3); $$=ret; }
        | pth ':' ':' id { char *ret; asprintf(&ret, "%s::%s", $1, $4); $$=ret; }

id:       ID { $$=strdup(yytext); }

string:   STRING { $$=strdup(yytext); }

%%
#include <stdio.h>

void yyerror(char const *s)
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}

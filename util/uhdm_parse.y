%token <str> ID
%token CONSTANT STRING_LITERAL BACK START END NL
%token  Restored
%token  parent
%token INT
%token always
%token assignment
%token begin
%token clk
%token constant
%token design
%token event_control
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
%token vpiLhs
%token vpiName
%token vpiProcess
%token vpiRhs
%token vpiStmt
%token vpiTopModule

%start start

%{
#include <string.h>
#include "../../util/uhdm_parse.h"

static void *design_ext;
static void *module_ext;
static void *proc_ext;
static void *bb_ext;
static void *proc_always_ext;
static void *event_control_ext;
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
  struct module_object *mod;
  struct cst_object *cst;
  struct asgn_object *asgn;
  struct always_object *alw;
  struct bgn_object *bgn;
  struct pkg_object *pkg;
  struct attr_list *lst;
  }

%type <top> start
%type <str> id
%type <vpi> vpi
%type <ptr> back parent
%type <lst> attr attr_lst
%type <constant> CONSTANT
%type <mod> modobj
%type <event> evobj
%type <ref> refobj
%type <cst> cstobj
%type <bgn> bgnobj
%type <pkg> pkgobj
%type <alw> alwobj
%type <asgn> asgnobj
%%

start: id '.' id ':' id design id '-' id ':' START vpi attr_lst { top.typ = TOP_OBJ; top.vtop = $12; top.attr = $13; $$ = &top; if (verbose_parse) printf("top=%p, %p\n", $12, $13); }
	;

vpi:      vpiDefName ':' id { $$ = VPI(DEF_NAME, (void *)$3, 0); }
	| vpiName ':' id { $$ = VPI(NAME, (void *)$3, 0); }
	| vpiFullName ':' id { $$ = VPI(FULL_NAME, (void *)$3, 0); }
	| vpiTopModule ':' CONSTANT { $$ = VPI(TOP_MOD, NULL, $3); }
	| vpiProcess ':' NL BACK back { $$ = VPI(PROC, $5, 0); if (verbose_parse) printf("proc: %p, back = %p\n", $$, $5); }
	| uhdmallPackages ':' NL BACK back {  $$ = VPI(ALL_PACK, $5, 0); if (verbose_parse) printf("allpack: %p, back = %p\n", $$, $5); }
	| uhdmallModules ':' NL BACK back { $$ = VPI(ALL_MOD, $5, 0); if (verbose_parse) printf("allmod: %p, back = %p\n", $$, $5); }
	| design ':' '(' id ')' { $$ = VPI(DESIGN, (void *)$4, 0); if (verbose_parse) printf("design: %p, id = %s\n", $$, $4); }
	| vpiAlwaysType ':' CONSTANT { $$ = VPI(ALWAYS, NULL, $3); if (verbose_parse) printf("always_type: %p\n", $$); }
	| vpiStmt ':' NL BACK back { $$ = VPI(STMT, $5, 0); if (verbose_parse) printf("stmt: %p, back = %p\n", $$, $5); }
	| vpiCondition ':' NL BACK back { $$ = VPI(COND, $5, 0); }
	| vpiLhs ':' NL BACK back { $$ = VPI(LHS, $5, 0); }
	| vpiRhs ':' NL BACK back { $$ = VPI(RHS, $5, 0); }
	| INT ':' CONSTANT { $$ = VPI(CST, NULL, $3); }

attr:     START attr_lst opt_end { int cnt; $$ = rev_lst ($2, &cnt, verbose_parse); if (verbose_parse) printf("attr: %p, rev = %p (len=%d)\n", $2, $$, cnt); }

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
	| evobj { $$ = $1;  if (verbose_parse) printf("back ev_obj: %p, ev_attr: %p\n", $1, $1->attr); }
	| refobj { $$ = $1; }
	| cstobj { $$ = $1; }
	| asgnobj { $$ = $1; }
	| bgnobj { $$ = $1; if (verbose_parse) printf("back bgn_obj: %p, bgn_attr: %p\n", $1, $1->attr); }
	| pkgobj { $$ = $1; }
	| alwobj { $$ = $1; }
;

pkgobj:	  package ':' id '(' id ')' attr { pkg_object *ptr = malloc(sizeof(pkg_object)); ptr->typ=PKG_OBJ; ptr->id1 = $3; ptr->id2 = $5; $$=ptr; }

bgnobj:	  begin ':' attr { bgn_object *ptr = malloc(sizeof(bgn_object)); ptr->typ=BGN_OBJ; ptr->attr = $3; $$ = ptr; }

cstobj:	  constant ':' attr { cst_object *ptr = malloc(sizeof(cst_object)); ptr->typ=CST_OBJ; ptr->attr = $3; $$ = ptr; }

alwobj:   always ':' attr { always_object *ptr = malloc(sizeof(always_object)); ptr->typ=ALWYS_OBJ; ptr->attr = $3; $$ = ptr; }

asgnobj:  assignment ':' attr { asgn_object *ptr = malloc(sizeof(asgn_object)); ptr->typ=ASGN_OBJ; ptr->attr = $3; $$ = ptr; }

refobj:	  ref_obj ':' '(' id ')' attr { ref_object *ptr = malloc(sizeof(ref_object)); ptr->typ=REF_OBJ; ptr->id = $4; ptr->attr = $6; $$ = ptr; }

evobj:	  event_control ':' attr { event_object *ptr = malloc(sizeof(event_object)); ptr->typ=EV_OBJ; ptr->attr = $3; $$ = ptr; if (verbose_parse) printf("ev_obj: %p, ev_attr: %p\n", ptr, $3); }

modobj:	  module ':' id '(' id ')' id '.' id ':' CONSTANT ':' ',' parent ':' id attr { module_object *ptr = malloc(sizeof(module_object)); 
            ptr->typ=MOD_OBJ; ptr->id1 = $3; ptr->id2 = $5; ptr->id3 = $7; ptr->id4 = $9; ptr->lin = $11; ptr->par = $14; ptr->id5 = $16; ptr->attr = $17; $$ = ptr; }

id:       ID { $$=strdup(yytext); }

%%
#include <stdio.h>

void yyerror(char const *s)
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define YYDEBUG 1

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kind { UNKNOWN, REF_OBJ, EV_OBJ, MOD_OBJ, CST_OBJ, ASGN_OBJ, BGN_OBJ, PKG_OBJ, ALWYS_OBJ, TOP_OBJ } kind_e;
typedef enum vtyp { VUNKNOWN, DEF_NAME, NAME, FULL_NAME, TOP_MOD, PROC, ALL_PACK, ALL_MOD, DESIGN, ALWAYS, STMT, COND, LHS, RHS, CST } typ_e;
typedef struct cast_obj { enum kind typ; } cast_obj;
typedef struct vpi_obj { enum vtyp typ; struct cast_obj *ptr; int cst; } vpi_obj;
typedef struct attr_list { enum kind typ; struct vpi_obj *itm; struct attr_list *nxt; } attr_list;
typedef struct ref_object { enum kind typ; char *id; attr_list *attr; } ref_object;
typedef struct event_object { enum kind typ; attr_list *attr; } event_object;
typedef struct cst_object { enum kind typ; attr_list *attr; } cst_object;
typedef struct always_object { enum kind typ; attr_list *attr; } always_object;
typedef struct bgn_object { enum kind typ; attr_list *attr; } bgn_object;
typedef struct pkg_object { enum kind typ; char *id1, *id2; } pkg_object;
typedef struct asgn_object { enum kind typ; attr_list *attr; } asgn_object;
typedef struct module_object { enum kind typ; char *id1, *id2, *id3, *id4, *id5; int lin; void *par; attr_list *attr; } module_object;
typedef struct top_object { enum kind typ; vpi_obj *vtop; attr_list *attr; } top_object;

extern top_object top; 
extern int old_code, verbose_parse, verbose_parse, verbose_mode, echo_mode, column;

void count(void);
void yyerror(char const *s);
void comment(void);
void error(const char *s);
int check_type(void);
int yyparse (void);
int yylex (void);
void yyset_in  ( FILE * );
const char *from_code(int code);

void build_from_file(void);
void *build_design_ext(const char *nam);
void *build_module_ext(void *d);
void *build_processes_ext(void);
void *build_begin_block_ext(void);
void *build_always_ext(void *m);
void *build_posedge_ext(const char *nam);
void *build_out_ext(void* at, const char *out);
const char *buildSimpleAlwaysDesign_ext(void *d, void *m, void *processes, const char *P0);

attr_list *rev_lst(attr_list *lst, int *, int);
void dump_lst(attr_list *lst);
void dump_kind(const struct cast_obj *ptr);
void dump_lst(attr_list *lst);

const char *symbol_name (int yysymbol);
int is_verbose_mode(void);
int is_echo_mode(void);

extern char *yytext;
extern int yydebug;

#define VPI(vtyp, vptr, vcst) vtmp = malloc(sizeof(vpi_obj)), vtmp->typ = vtyp, vtmp->ptr = vptr, vtmp->cst = vcst, vtmp

#ifdef __cplusplus
};
#endif

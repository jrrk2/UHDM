#include <iostream>
#include <stdlib.h>
#include "uhdm_parse.h"
#include "uhdm/uhdm.h"
#include "uhdm/vpi_visitor.h"

using namespace UHDM;

#include "uhdm/vpi_visitor.h"

// This builds a simple design:
// module m1;
//   always @(posedge clk)
//     out = 1;
//   end
// endmodule

static Serializer s;
static design *d;
static module *m1;

void dump_proc(VectorOfprocess_stmt *processes, const struct cast_obj *ptr);

static design *build_design(const char *nam)
{
  // Design building
  puts("build_design");
  d = s.MakeDesign();
  d->VpiName(nam);
  return d;
}

static module *build_module(design *d)
{
  puts("build_module");
  m1 = s.MakeModule();
  m1->VpiTopModule(true);
  m1->VpiDefName("M1");
  m1->VpiName("u1");
  m1->VpiParent(d);
  m1->VpiFile("fake1.sv");
  m1->VpiLineNo(10);
  return m1;
}

void build_processes(const struct cast_obj *ptr)
{
  puts("build_processes");
  VectorOfprocess_stmt *processes = s.MakeProcess_stmtVec();
  dump_proc(processes, ptr);
  puts("attach processes");
  m1->Process(processes);
}

static event_control *build_posedge(const char *nam)
{
  tchk_term *posedge_clk;
  event_control *at;
  // @(posedge clk)
  at = s.MakeEvent_control();
  ref_obj *clk = s.MakeRef_obj();
  clk->VpiName(nam);
  posedge_clk = s.MakeTchk_term();
  posedge_clk->VpiEdge(vpiPosedge);
  posedge_clk->Expr(clk);
  VectorOfany *simple_exp_vec = s.MakeAnyVec();
  simple_exp_vec->push_back(posedge_clk);
  clk->VpiUses(simple_exp_vec);
  at->VpiCondition(clk);
  return at;
}

void build_out(VectorOfany *statements, event_control* at, const char *out)
{
  s_vpi_value val;
  // out = 1;
  ref_obj* lhs_rf = s.MakeRef_obj();
  lhs_rf->VpiName(out);
  assignment* assign1 = s.MakeAssignment();
  assign1->Lhs(lhs_rf);
  constant* c1 = s.MakeConstant();
  val.format = vpiIntVal;
  val.value.integer = 1;
  c1->VpiValue(VpiValue2String(&val));
  assign1->Rhs(c1);
  at->Stmt(assign1);
  statements->push_back(at);
}

const char *buildSimpleAlwaysDesign(design *d, module *m, VectorOfprocess_stmt *processes, const char *P0)
{
return "";
}

void build_from_default()
{
  design *d = build_design("design_process");
  module *m = build_module(d);
  VectorOfprocess_stmt * proc = s.MakeProcess_stmtVec();
  begin *bb = s.MakeBegin();
  always *proc_always = s.MakeAlways();
  proc_always->Module(m);
  proc_always->VpiAlwaysType(vpiAlways);
  proc_always->VpiAlwaysType(vpiAlwaysComb);
  proc_always->VpiAlwaysType(vpiAlwaysFF);
  proc_always->VpiAlwaysType(vpiAlwaysLatch);
  proc_always->Stmt(bb);
  proc->push_back(proc_always);
  event_control* at = build_posedge("clk");
  VectorOfany *statements = s.MakeAnyVec();
  build_out(statements, at, "out");
  bb->Stmts(statements);
  const char *orig = buildSimpleAlwaysDesign(d, m, proc, "P0");

  const std::string filename = "surelog_process.uhdm";
  s.Save(filename);

  const std::vector<vpiHandle> restoredDesigns = s.Restore(filename);
  const std::string restored = visit_designs(restoredDesigns);
}

top_object top;

int old_code, verbose_parse, verbose_mode, echo_mode;

int is_verbose_parse(void)
{
const char *env = getenv ("UHDM_PARSER_VERBOSE"); 
verbose_parse = env ? atoi(env) : 0;
return verbose_parse;
}

int is_verbose_mode(void)
{
const char *env = getenv ("UHDM_LEXER_VERBOSE"); 
verbose_mode = env ? atoi(env) : 0;
return verbose_mode;
}

int is_echo_mode(void)
{
const char *env = getenv ("UHDM_LEXER_ECHO");
echo_mode = env ? atoi(env) : 0;
return echo_mode;
}

void error(const char *s)
{
  fprintf(stderr, "%s\n", s);
}

const char *id_vpi(struct vpi_obj *ptr)
{
switch(ptr->typ)
  {
  case VUNKNOWN: return ("VUNKNOWN"); break;
  case DEF_NAME: return ("DEF_NAME"); break;
  case NAME: return ("NAME"); break;
  case FULL_NAME: return ("FULL_NAME"); break;
  case TOP_MOD: return ("TOP_MOD"); break;
  case PROC: return ("PROC"); break;
  case ALL_PACK: return ("ALL_PACK"); break;
  case ALL_MOD: return ("ALL_MOD"); break;
  case DESIGN: return ("DESIGN"); break;
  case ALWAYS: return ("ALWAYS"); break;
  case STMT: return ("STMT"); break;
  case COND: return ("COND"); break;
  case LHS: return ("LHS"); break;
  case RHS: return ("RHS"); break;
  case CST: return ("CST"); break;
  default: abort();
  }
}

void dump_design(const char *ptr)
{
d = build_design(ptr);
}

void dump_vunknown(const void *ptr)
{
puts("unknown");
}

void dump_def_name(const char *ptr)
{
puts(ptr);
}

void dump_name(const char *ptr)
{
puts(ptr);
}

void dump_full_name(const char *ptr)
{
puts(ptr);
}

void dump_top_mod(int cst)
{
printf("%d\n", cst);
}

void dump_always_vpi(const int cst)
{
printf("%d\n", cst);
}

void dump_cst(int cst)
{
printf("%d\n", cst);
}

void all_mod(const struct cast_obj *ptr)
{
  VectorOfmodule* v1;
  puts("begin_all_mod");
  dump_kind(ptr);
  v1 = s.MakeModuleVec();
  v1->push_back(m1);
  d->AllModules(v1);
  puts("end_all_mod");
}

void all_pkg()
{
  package* p1;
  VectorOfpackage* v3;
  p1 = s.MakePackage();
  p1->VpiDefName("P0");
  v3 = s.MakePackageVec();
  v3->push_back(p1);
  d->AllPackages(v3);
}

char *all_des()
{
  vpiHandle dh;
  std::vector<vpiHandle> designs;
  vpiHandle obj_h;
  char name[]{"u1"};
  dh = s.MakeUhdmHandle(uhdmdesign, d);
  designs.push_back(dh);

  obj_h = vpi_handle_by_name(name, dh);
  const std::string orig = visit_designs(designs);
  return strdup(orig.c_str());
}

void begin_stmts()
{
puts("begin_stmts");
}

void end_stmts()
{
puts("end_stmts");
}

void dump_vpi(struct vpi_obj *ptr)
{
switch(ptr->typ)
  {
  case VUNKNOWN: dump_vunknown(ptr->ptr); break;
  case DEF_NAME: dump_def_name((char *)(ptr->ptr)); break;
  case NAME: dump_name((char *)(ptr->ptr)); break;
  case FULL_NAME: dump_full_name((char *)(ptr->ptr)); break;
  case TOP_MOD: dump_top_mod(ptr->cst); break;
  case PROC: build_processes(ptr->ptr); break;
  case ALL_PACK: dump_kind(ptr->ptr); all_pkg(); break;
  case ALL_MOD: all_mod(ptr->ptr); break;
  case DESIGN: dump_design((char *)(ptr->ptr)); break;
  case ALWAYS: dump_always_vpi(ptr->cst); break;
  case STMT: begin_stmts(); dump_kind(ptr->ptr); end_stmts(); break;
  case COND: dump_kind(ptr->ptr); break;
  case LHS: dump_kind(ptr->ptr); break;
  case RHS: dump_kind(ptr->ptr); break;
  case CST: dump_cst(ptr->cst); break;
  default: abort();
  }
}

void dump_ref(struct ref_object *ptr)
{
printf("REF_OBJ: %p\n", ptr->attr);
dump_lst(ptr->attr);
event_control* at = build_posedge("clk");
}

void dump_event(struct event_object *ptr)
{
  printf("EV_OBJ default handler: %p\n", ptr->attr);
  dump_lst(ptr->attr);
}

void dump_module(struct module_object *ptr)
{
printf("MOD_OBJ: %s %s %s %s %s %d %p\n", ptr->id1, ptr->id2, ptr->id3, ptr->id4, ptr->id5, ptr->lin, ptr->par);
m1 = build_module(d);
dump_lst(ptr->attr);
}

void dump_const(struct cst_object *ptr)
{
printf("CST_OBJ: %p\n", ptr->attr);
dump_lst(ptr->attr);
}

assignment *dump_assign(struct asgn_object *ptr)
{
  assignment* assign1 = NULL;
  printf("ASGN_OBJ: %p\n", ptr->attr);

  struct vpi_obj *ptr0 = ptr->attr->itm;
  struct vpi_obj *ptr1 = ptr->attr->nxt->itm;
  if (ptr0->typ == RHS && ptr1->typ == LHS)
    {
      const struct cast_obj *kptr0 = ptr0->ptr;
      const struct cast_obj *kptr1 = ptr1->ptr;
      puts("LHS,RHS");
      if (kptr0->typ == CST_OBJ && kptr1->typ == REF_OBJ)
        {
          cst_object *cptr = (cst_object *)kptr0;
          ref_object *rptr = (ref_object *)kptr1;
          struct vpi_obj *vptr0 = cptr->attr->itm;
          struct vpi_obj *vptr1 = rptr->attr->itm;
          puts("CSR,REF");
          if (vptr0->typ == CST && vptr1->typ == NAME)
            {
              int cst = vptr0->cst;
              const char *nam = (char *)(vptr1->ptr);
              printf("CST: %d\n", cst);
              printf("NAME: %s\n", nam);
              // LHS
              ref_obj *lhs_rf = s.MakeRef_obj();
              lhs_rf->VpiName(nam);
              // RHS
              constant *c1 = s.MakeConstant();
              s_vpi_value val;
              val.format = vpiIntVal;
              val.value.integer = cst;
              c1->VpiValue(VpiValue2String(&val));
              // assign
              assign1 = s.MakeAssignment();
              assign1->Lhs(lhs_rf);
              assign1->Rhs(c1);
              puts("assignment completed");
              }
          else
            {
              dump_vpi(vptr0);
              dump_vpi(vptr1);
            }
          }
      else
        {
          dump_kind(kptr0);
          dump_kind(kptr1);
          }
    }
  else
    {
      dump_vpi(ptr0);
      dump_vpi(ptr1);
    }
return assign1;
}

void dump_begin(struct bgn_object *ptr)
{
  printf("BGN_OBJ: %p\n", ptr->attr);
  puts("build_begin_block");
  begin *begin_block = s.MakeBegin();
  dump_lst(ptr->attr);
}

void dump_pkg(struct pkg_object *ptr)
{
printf("PKG_OBJ: %s %s\n", ptr->id1, ptr->id2);
}

void dump_always(VectorOfprocess_stmt *processes, struct always_object *ptr)
{
  puts("ALWYS_OBJ"); 
  // always @(posedge clk) begin
  always *proc_always = s.MakeAlways();
  proc_always->Module(m1);
  proc_always->VpiAlwaysType(vpiAlways);
  proc_always->VpiAlwaysType(vpiAlwaysComb);
  proc_always->VpiAlwaysType(vpiAlwaysFF);
  proc_always->VpiAlwaysType(vpiAlwaysLatch);
  attr_list *lst = ptr->attr;
  if (lst->itm->typ == ALWAYS && lst->nxt->itm->typ == STMT && lst->nxt->nxt == NULL && lst->nxt->itm->ptr->typ == BGN_OBJ)
    {
      struct bgn_object *ptr = (bgn_object *)(lst->nxt->itm->ptr);
      puts("ALWAYS,STMT");
      dump_always_vpi(lst->itm->cst);
      begin *begin_block = s.MakeBegin();
      attr_list *lst = ptr->attr;
      if (lst->itm->typ == STMT && lst->nxt == NULL && lst->itm->ptr->typ == EV_OBJ)
        {
          struct event_object *ptr = (event_object *)(lst->itm->ptr);
          attr_list *lst = ptr->attr;
          puts("EV_OBJ,NULL");
          if (lst->itm->typ == COND && lst->nxt->itm->typ == STMT && lst->nxt->nxt == NULL && lst->itm->ptr->typ == REF_OBJ && lst->nxt->itm->ptr->typ == ASGN_OBJ)
            {
              puts("REF_OBJ,ASGN_OBJ");
              struct ref_object *rptr = (ref_object *)(lst->itm->ptr);
              struct asgn_object *aptr = (asgn_object *)(lst->nxt->itm->ptr);
              attr_list *rlst = rptr->attr;
              event_control* at;
              VectorOfany *statements = s.MakeAnyVec();
              if (rlst->itm->typ == NAME && rlst->nxt == NULL)
                {
                  const char *nam = (char *)(rlst->itm->ptr);
                  puts(nam);
                  at = build_posedge(nam);
                }
              else while (rlst)
                {
                  printf("Ref list entry %s: ", id_vpi(rlst->itm));
                  dump_vpi(rlst->itm);
                  rlst = rlst->nxt;
                }
              at->Stmt(dump_assign(aptr));
              statements->push_back(at);
              begin_block->Stmts(statements);
            }
          else while (lst)
            {
              printf("Event list entry %s: ", id_vpi(lst->itm));
              dump_vpi(lst->itm);
              lst = lst->nxt;
            }
        }
      else while (lst)
        {
          printf("Stmt list entry %s: ", id_vpi(lst->itm));
          dump_vpi(lst->itm);
          lst = lst->nxt;
        }
     proc_always->Stmt(begin_block);
    }
  else while (lst)
    {
      printf("List entry %s: ", id_vpi(lst->itm));
      dump_vpi(lst->itm);
      lst = lst->nxt;
    }
  processes->push_back(proc_always);
}

void dump_kind(const struct cast_obj *ptr)
{
switch (ptr->typ)
  {
  case UNKNOWN: puts("UNKNOWN"); break;
  case REF_OBJ: dump_ref((ref_object *)ptr); break;
//  case EV_OBJ: dump_event((event_object *)ptr); break;
  case MOD_OBJ: dump_module((module_object *)ptr); break;
  case CST_OBJ: dump_const((cst_object *)ptr); break;
  case ASGN_OBJ: dump_assign((asgn_object *)ptr); break;
  case BGN_OBJ: dump_begin((bgn_object *)ptr); break;
  case PKG_OBJ: dump_pkg((pkg_object *)ptr); break;
//  case ALWYS_OBJ: dump_always((always_object *)ptr); break;
  default:
    printf("dump_kind: kind not handled: %d\n", ptr->typ);
    abort();
  }
}

void dump_proc(VectorOfprocess_stmt *processes, const struct cast_obj *ptr)
{
switch (ptr->typ)
  {
  case UNKNOWN: puts("UNKNOWN"); break;
  case REF_OBJ: dump_ref((ref_object *)ptr); break;
  case EV_OBJ: dump_event((event_object *)ptr); break;
  case MOD_OBJ: dump_module((module_object *)ptr); break;
  case CST_OBJ: dump_const((cst_object *)ptr); break;
  case ASGN_OBJ: dump_assign((asgn_object *)ptr); break;
  case BGN_OBJ: dump_begin((bgn_object *)ptr); break;
  case PKG_OBJ: dump_pkg((pkg_object *)ptr); break;
  case ALWYS_OBJ: dump_always(processes, (always_object *)ptr); break;
  default:
    printf("dump_proc: kind not handled: %d\n", ptr->typ);
    abort();
  }
}

void dump_lst(attr_list *lst)
{
  int cnt = 0;
  while (lst)
    {
      printf("List entry %d, %s: ", cnt++, id_vpi(lst->itm));
      dump_vpi(lst->itm);
      lst = lst->nxt;
    }
}

attr_list *rev_lst(attr_list *lst, int *cnt, int verbose)
{
  attr_list *rlst = NULL;
  *cnt = 0;
  while (lst)
    {
      attr_list *rlst2 = (attr_list *)malloc(sizeof(attr_list));
      rlst2->itm = lst->itm;
      rlst2->nxt = rlst;
      rlst = rlst2;
      if (verbose) puts(id_vpi(rlst->itm));
      lst = lst->nxt;
      ++*cnt;
    }
  return rlst;
}

void dump_tree(struct top_object *ptr)
{
  int cnt;
  puts("dumping");
  dump_vpi(ptr->vtop);
  dump_lst(rev_lst(ptr->attr, &cnt, 0));
}

int main(int argc, char **argv)
{
FILE *yyin;
if (argc > 1)
  {
    yyin = fopen(argv[1], "r");
    yyset_in(yyin);
  }
else
  puts ("Scanning from stdin");
yydebug=argc > 2;
is_verbose_parse();
is_verbose_mode();
is_echo_mode();
yyparse();
if (top.vtop)
  {
  dump_tree(&top);
  puts(all_des()); 
  }
else
  {
  printf("Last symbol = %s\n", from_code(old_code));
  }
}

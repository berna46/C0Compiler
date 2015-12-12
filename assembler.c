#include "printMips.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "assembler.h"
#include <search.h>
#define MAX_SIZE 50

// variaveis para crirar nomes de registos temporarios
int i = 0;
int lb = 0;
char *final_reg = "";

// tabela de símbolos
int init_hash = 0;
ENTRY *symbolTable;
// tabela de simbolos

void reverse(char s[]) {
  int i, j;
  char c;

  for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

void itoa(int n, char s[]) {
  int i, sign;

  if ((sign = n) < 0)
    n = -n;
  i = 0;
  do {
    s[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse(s);
}

Address makeVal(int n) {
  Address a = malloc(sizeof(*a));
  a->AddrKind = Int;
  a->content.val = n;
  return a;
}

Address makeVar(char *c) {
  Address a = malloc(sizeof(*a));
  a->AddrKind = String;
  a->content.var = c;
  return a;
}

Address makeEmpty() {
  Address a = malloc(sizeof(*a));
  a->AddrKind = Empty;
  return a;
}

Address makeNewVar() {
  char *v = malloc(sizeof(char) * 3);
  char *var = malloc(sizeof(char) * 4);
  var[0] = 't';
  var[1] = '\0';
  // supondo que não serão usados 100 registos
  itoa(i, v);
  strcat(var, v);
  i++;
  printf("reg: %s\n", var);
  // char *r = "t#";
  return makeVar(var);
}

Address makeNewLabel() {
  char *v = malloc(sizeof(char) * 3);
  char *var = malloc(sizeof(char) * 4);
  var[0] = 'l';
  // supondo que não serão usados 100 registos
  itoa(lb, v);
  strcat(var, v);
  lb++;
  return makeVar(var);
}

TAC makeTAC(OpKind o, Address a1, Address a2, Address a3) {
  TAC c = malloc(sizeof(*c));
  c->op = o;
  c->addr1 = a1;
  c->addr2 = a2;
  c->addr3 = a3;
  return c;
}

TACList makeTACList(TAC h, TACList t) {
  TACList c = malloc(sizeof(*c));
  c->head = h;
  c->tail = t;
  return c;
}

Pair makePair(Address a, TACList l) {
  Pair p = malloc(sizeof(*p));
  p->addr = a;
  p->clist = l;
  return p;
}

Output makeOutput(char *c, TACList l) {
  Output o = malloc(sizeof(*o));
  o->var = c;
  o->clist = l;
  return o;
}

TACList append(TACList cl1, TACList cl2) {

  // printf("APPENDING TIME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  TACList cl3 = malloc(sizeof(TACList));
  if (cl1 != NULL) {
    cl3 = cl1;
    int k = 0;
    while (cl3->tail != NULL) {
      printf("k: %d\n", k);

      cl3 = cl3->tail;
      ++k;
    }
    cl3->tail = cl2;
  } else
    cl1 = cl3;
  return cl1;
}

OpKind getOp(A_AOper o) {
  switch (o) {
  case OpPlus:
    return A_Plus;
    break;
  case OpMinus:
    return A_Minus;
    break;
  case OpTimes:
    return A_Times;
    break;
  case OpDiv:
    return A_Div;
    break;
  default:
    return -1;
  }
}

OpKind getBop(A_BOper o) {
  switch (o) {
  case OpEQ:
    return A_BEQ;
  case OpG:
    return A_BGT;
  case OpGEQ:
    return A_BGE;
  case OpL:
    return A_BLT;
  case OpLEQ:
    return A_BLE;
  case OpNOTEQ:
    return A_BNE;
  case OpAND:
    return A_AND;
  case OpOR:
    return A_OR;
  default:
    return -1;
  }
}

Pair compile_exp(A_exp e) {
  Pair auxA = malloc(sizeof(*auxA)), auxB = malloc(sizeof(*auxB)),
       p = malloc(sizeof(*p)), res = malloc(sizeof(*res));
  TACList list = malloc(sizeof(*list)), list3 = malloc(sizeof(*list3)),
          tmp = malloc(sizeof(*tmp));
  Address t0 = malloc(sizeof(*t0));
  TAC elem = malloc(sizeof(*elem));
  OpKind op;
  switch (e->kind) {
  case A_BopExp:
    printf("COMPILE_Bool_EXP_0\n");
    t0 = makeNewVar();                  // var. resultado
    auxA = compile_exp(e->u.opB.left);  // ramo esq.
    auxB = compile_exp(e->u.opB.right); // ramo dir.
    op = getBop(e->u.opB.oper);
    elem = makeTAC(op, t0, auxA->addr, auxB->addr); // INSTRUÇÃO FINAL
    // APPEND esq. ++ dir.
    if (auxA->clist != NULL && auxB->clist != NULL) {
      printf("lol\n");
      list3 = append(auxA->clist, auxB->clist);
    } else if (auxA->clist != NULL) {
      printf("lol2\n");
      list3 = auxA->clist;
    } else {
      printf("lol3\n");
      list3 = auxB->clist; // pode ser NULL
    }
    printf("COMPILE_BOOL_EXP_1\n");
    tmp = makeTACList(elem, NULL); // exp. final
    if (list3 != NULL)
      list = append(list3, tmp);
    else
      list = tmp;
    printf("APPEND_F_END\n");
    res = makePair(t0, list);
    printf("COMPILE_BOOL_EXP_END res->t0: %s\n", res->addr->content.var);
    //última var. ()é usada no salto)
    if (strcmp(final_reg, "") != 0)
      free(final_reg);
    final_reg = malloc(sizeof(char) * 4);
    final_reg = res->addr->content.var;
    return res;
    break;
  case A_AopExp:
    printf("COMPILE_EXP_0\n");
    t0 = makeNewVar();                  // var. resultado
    auxA = compile_exp(e->u.opA.left);  // ramo esq.
    auxB = compile_exp(e->u.opA.right); // ramo dir.
    op = getOp(e->u.opA.oper);
    elem = makeTAC(op, t0, auxA->addr, auxB->addr); // INSTRUÇÃO FINAL
    // APPEND esq. ++ dir.
    if (auxA->clist != NULL && auxB->clist != NULL) {
      printf("lol\n");
      list3 = append(auxA->clist, auxB->clist);
    } else if (auxA->clist != NULL) {
      printf("lol2\n");
      list3 = auxA->clist;
    } else {
      printf("lol3\n");
      list3 = auxB->clist; // pode ser NULL
    }
    // printf("COMPILE_EXP_1\n");
    tmp = makeTACList(elem, NULL); // exp. final
    if (list3 != NULL)
      list = append(list3, tmp);
    else
      list = tmp;
    // printf("APPEND_F_END\n");
    res = makePair(t0, list);
    printf("COMPILE_EXP_END res->t0: %s\n", res->addr->content.var);
    return res;
    break;

  case A_intExp:
    printf("- - - COMPILE INT: %d -> ", e->u.intt);
    p = makePair(makeVal(e->u.intt), NULL);
    printf("%d\n", p->addr->content.val);
    char *var = malloc(sizeof(char) * 2);
    itoa(p->addr->content.val, var);
    final_reg = var;
    return p;
    printf("int to str: %s\n", final_reg);
    break;

  case A_varExp:
    printf("- - - COMPILE VAR: %s -> ", e->u.var);
    p = makePair(makeVar(e->u.var), NULL);
    printf("%s\n", p->addr->content.var);
    final_reg = p->addr->content.var;
    return p;
    break;
  case A_boolExp:
    printf("COMPILE BOOL VAR: %s -> ", e->u.var);
    char *v = malloc(sizeof(char) * 2);
    int val = 0;
    if (e->u.booll == BOOL_TRUE)
      val = 1;
    p = makePair(makeVal(e->u.intt), NULL);
    printf("val: %d", val);
    itoa(val, v);
    final_reg = v;
    return p;
    break;
  default:
    return NULL;
    break;
  }
}

/********£@£¹ COMPLETAR ¹@£§¹@**********/
Pair compile(I_list il) {
  printf("GO\n");
  printf("GO\n");
  printf("GO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
         "!!!!!!!!!!!!!!!!!!!!!!!!\n");

  Pair tl = malloc(sizeof(*tl));
  switch (il->kind) {
  case A_EXP_:
    tl = compile_exp(il->head.a_exp);
    break;
  case CMD_:
    tl = compile_cmd(il->head.cmd);
    break;
  case DECL_:
    compile_decl(il->head.decl);
    break;
  }
  // print_TACLIST(tl->clist);
  if (il->tail != NULL) {
    Pair aux = compile(il->tail);
    tl->clist = append(tl->clist, aux->clist);
  }
  // print_TACLIST(tl->clist);
  return tl;
}

void create_hasht() {
  hcreate(211);
  init_hash = 1;
}

void compile_decl(DECL decl) {
  // verifica se a tabela de símbolos já foi inicializada
  if (init_hash == 0)
    create_hasht();
  ENTRY init;
  // chave
  init.key = decl->var;
  // valor
  if (decl->type == INT_TYPE)
    init.data = (void *)INT_TYPE;
  else
    init.data = (void *)BOOL_TYPE;
  symbolTable = hsearch(init, ENTER);
  if (symbolTable == NULL) {
    fprintf(stderr, "entry failed\n");
    exit(EXIT_FAILURE);
  }
}

// compila um só comando
Pair compile_cmd(CMD cmd) {
  printf("COMPILE_CMD\n");

  Pair p = malloc(sizeof(*p));
  TACList tl = malloc(sizeof(*tl));
  switch (cmd->kind) {
  case IF_KIND:
    tl = compile_if(cmd);
    break;
  case ASSIGN_KIND:
    tl = compile_ass(cmd);
    break;
  /*  case WHILE_KIND:
      tl = compile_while(cmd);
      break;*/
  default:
    break;
  }
  p = makePair(NULL, tl);
  return p;
}

/*// compila comando WHILE
TACList compile_while(CMD wh) {
  Pair e_aux = compile_exp(wh->u.w.while_);
  TACList tl = compile(wh->u.w.while_I_list_);
  // LABEL "!#!"§¹@§
  return tl;
}*/

TACList compile_ass(CMD d) {
  printf("ASSSS ASADSAASASSSSSSSSSSSSSSSSS _F_ SDF EW£¹@§@¹§½¹\n");
  Pair p_exp = compile_exp(d->u.ass.assignment_);
  Address addr1 = makeVar(d->u.ass.var_);
  Address addr2 = p_exp->addr;
  TAC t = makeTAC(A_Asn, addr1, addr2, NULL);
  TACList l = makeTACList(t, NULL);
  TACList aux = malloc(sizeof(*aux));
  if (p_exp->clist == NULL) {
    p_exp->clist = l;
    aux = p_exp->clist;
  } else {
    aux = append(p_exp->clist, l);
  }
  return aux;
}

TACList compile_if(CMD ift) {
  printf("COMPILE_IF\n");
  TACList ilb = malloc(sizeof(*ilb)), jlb = malloc(sizeof(*jlb)),
          elb = malloc(sizeof(*elb)); // fl = malloc(sizeof(*fl));
  Pair then_list = malloc(sizeof(*then_list)),
       else_list = malloc(sizeof(*else_list));
  // IF LABEL
  Pair p_exp = compile_exp(ift->u.if_else.if_);
  // cria if_label e coloca exp. na cauda da label
  ilb = makeTACList(makeTAC(Label, makeNewLabel(), NULL, NULL), p_exp->clist);
  printf("new IF label: %s\n", ilb->head->addr1->content.var);
  // ilb->tail = p_exp->clist;
  // IF_FALSE
  jlb->head = makeTAC(On_False, makeVar(final_reg), makeNewLabel(),
                      NULL); // cria jump_label
  printf("final_reg: %s new JUMP label: %s\n", jlb->head->addr1->content.var,
         jlb->head->addr2->content.var);
  // adiciona jump_label ao fim da exp.
  ilb = append(ilb, jlb);
  // then statement
  Address end_if = makeNewLabel();
  if (ift->u.if_else.then_I_list_ != NULL) {
    then_list = compile(ift->u.if_else.then_I_list_);
    // adiciona jump ao fim das instruçoes (salta else)
    // VERIFICAR SE EXISTE else
    if (ift->u.if_else.else_I_list_ != NULL)
      then_list->clist =
          append(then_list->clist,
                 makeTACList(makeTAC(GoToLabel, end_if, NULL, NULL), NULL));
    ilb = append(ilb, then_list->clist);
  }
  // adiciona jump_label (else/fim do if)
  elb = makeTACList(
      makeTAC(Label, makeVar(jlb->head->addr2->content.var), NULL, NULL), NULL);
  ilb = append(ilb, elb);
  // else
  if (ift->u.if_else.else_I_list_ != NULL) {
    else_list = compile(ift->u.if_else.else_I_list_);
    // adiciona end_if ao fim da lista de instruções
    else_list->clist =
        append(else_list->clist,
               makeTACList(makeTAC(Label, end_if, NULL, NULL), NULL));
    ilb = append(ilb, else_list->clist);
  }
  print_TACLIST(ilb);
  printf("\n");
  return ilb;
}

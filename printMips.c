#include "assembler.h"
#include <stdio.h>

void print_Address(Address a) {
  // printf("PRINTING_ADRESS\n");
  switch (a->AddrKind) {
  case String:
    printf("%s", a->content.var);
    break;
  case Int:
    printf("%d", a->content.val);
    break;
  default:
    break;
  }
}

void print_TAC(TAC t) {
  //  printf("PRINTING_TAC\n" );
  switch (t->op) {
  case A_Plus:
    printf("Add ");
    print_Address(t->addr1);
    printf(", ");
    print_Address(t->addr2);
    printf(", ");
    print_Address(t->addr3);
    printf("\n");
    break;
  case A_Minus:
    printf("Sub ");
    print_Address(t->addr1);
    printf(", ");
    print_Address(t->addr2);
    printf(", ");
    print_Address(t->addr3);
    printf("\n");
    break;
  case A_Times:
    printf("Mul ");
    print_Address(t->addr1);
    printf(", ");
    print_Address(t->addr2);
    printf(", ");
    print_Address(t->addr3);
    printf("\n");
    break;
  case A_Div:
    printf("Div ");
    print_Address(t->addr1);
    printf(", ");
    print_Address(t->addr2);
    printf(", ");
    print_Address(t->addr3);
    printf("\n");
    break;
  case A_Asn:
    printf("lw ");
    print_Address(t->addr1);
    printf(", ");
    print_Address(t->addr2);
    // print_Address(t->addr3);
    printf("\n");
    break;
  case A_BEQ:
    printf("SL? ");
    print_Address(t->addr1);
    printf(", ");
    print_Address(t->addr2);
    printf(", ");
    print_Address(t->addr3);
    printf("\n");
    break;
  case Label:
    print_Address(t->addr1);
    printf(": ");
    break;
  case GoToLabel:
    printf("JUMP ");
    print_Address(t->addr1);
    printf("\n");
    break;
  case On_False:
    printf("BEQZ ");
    print_Address(t->addr1);
    printf(", ");
    print_Address(t->addr2);
    printf("\n");
  default:
    break;
  }
  //  printf("PRINT_TAC_END\n" );
}

void print_TACLIST(TACList tl) {
  // printf("PRINTING_TACList\n");
  print_TAC(tl->head);
  if (tl->tail != NULL) {
    print_TACLIST(tl->tail);
  }
}

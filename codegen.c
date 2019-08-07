
#include "9cc.h"

// syntax tree print function for debug
void tree_print(Node* node, int i){
  switch(node->kind){
  case ND_NUM:
    printf("%d:num:%d\n", i, node->val);
    return;
  case ND_LVAR:
    printf("%d:var:%c\n", i, node->offset/8+'a'-1);
    return;
  case ND_RETURN:
    printf("%d:return\n", i);
    tree_print(node->lhs, i+1);
    return;
  }
  
  ++i;
  tree_print(node->lhs, i);
  printf("- - - -\n");
  tree_print(node->rhs, i);

  --i;
  printf("-------\n");
  printf("%d:%d\n", i, node->kind);
  return;
  
}

// generate stack machine from the sytax tree
void gen(Node* node){
  switch(node->kind){
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);
    
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case ND_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }
    
  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch(node->kind){
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQU:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NEQ:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LWT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LEQ:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }
  printf("  push rax\n");

}

void gen_lval(Node* node){
  if(node->kind != ND_LVAR){
    printf("%d\n",node->kind);
    error("left hand side is not a variable");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");

}





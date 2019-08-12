
#include "9cc.h"

// syntax tree print function for debug
void tree_print(Node* node, int i){
  switch(node->kind){
  case ND_NUM:
    printf("%d:num:%d\n", i, node->val);
    return;
  case ND_LVAR:
    printf("%d:var:%c\n", i, node->offset/8+'a');
    return;
  case ND_RETURN:
    printf("%d:return\n", i);
    tree_print(node->lhs, i+1);
    return;
  case ND_WHILE:
    printf("%d:while\n", i);
    tree_print(node->cond, i+1);
    tree_print(node->then, i+1);
    return;
  case ND_FOR:
    printf("%d:for\n", i);
    printf("---init\n");
    tree_print(node->init, i+1);
    printf("---cond\n");
    tree_print(node->cond, i+1);
    printf("---end\n");
    tree_print(node->end, i+1);
    printf("---then\n");
    tree_print(node->then, i+1);
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
  int l_label_num = g_label_num;
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
  case ND_IF: // if(A) B;
    g_label_num++;
    gen(node->cond); // code for conditional expression i.e. A
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    if(!node->els){ // without "else"
      printf("  je .Lendif%d\n", l_label_num);
      gen(node->then); // code for resultant statement i.e. B
      printf(".Lendif%d:\n", l_label_num);
    }else{ // with "else"
      printf("  je .Lelse%d\n", l_label_num);
      gen(node->then); // code for "then" statement i.e. B
      printf("  jmp .Lendif%d\n", l_label_num); // skip "else" statement
      printf(".Lelse%d:\n", l_label_num); 
      gen(node->els); // code for "else" statement i.e. B
      printf(".Lendif%d:\n", l_label_num);
    }
    return;
  case ND_WHILE: // while(A) B
    g_label_num++;
    printf(".Lbeginwhile%d:\n", l_label_num);
    gen(node->cond); // code for conditional expression i.e. A
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lendwhile%d\n", l_label_num);
    gen(node->then); // code 
    printf("  jmp .Lbeginwhile%d\n", l_label_num); 
    printf(".Lendwhile%d:\n", l_label_num);
    return;
  case ND_FOR: // for(A;B;C)D
    g_label_num++;
    if(node->init != NULL){
      gen(node->init); // A
    }
    printf(".Lbeginfor%d:\n",l_label_num);
    if(node->cond != NULL){
      gen(node->cond); // B
    }else{
      printf("  push 1\n");
    }
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lendfor%d\n", l_label_num);
    gen(node->then); // D
    if(node->end != NULL){
      gen(node->end); // C
    }
    printf("  jmp .Lbeginfor%d\n", l_label_num); 
    printf(".Lendfor%d:\n",l_label_num);
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





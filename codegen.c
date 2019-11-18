#include "kfc.h"

// generate stack machine from the sytax tree
void gen(Node* node){
  int l_label_num = g_label_num;
  char str[VAR_NAME_SIZE];
  int arg_num = 0;
  unsigned int hash;
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
  case ND_LVAR_DEC:
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
  case ND_BLOCK:
    gen_node_list(node->comp_stmt);
    return;
  case ND_FUNC:
    g_label_num++;
    while(node->func_args && (arg_num < MAX_ARG_NUM) ){
      printf("  #evaluate argument %d\n", arg_num+1);
      gen(node->func_args->data);
      node->func_args = node->func_args->next;
      arg_num++;
    }
    for(int i=arg_num-1;i>=0;--i){
      printf("  pop %s #argument %d\n", ARG_REG[i], i+1);
    }
    if(node->len >= VAR_NAME_SIZE){
      error("Too long name of variable");
      return;
    }
    printf("  mov rax, rsp\n");
    printf("  and rax, 15\n");
    printf("  jnz .Lcall%d\n", l_label_num);
    printf("  mov rax, 0\n");
    printf("  call %s\n", node->name);
    printf("  jmp .Lend%d\n", l_label_num);
    printf(".Lcall%d:\n", l_label_num);
    printf("  sub rsp, 8\n");
    printf("  mov rax, 0\n");
    printf("  call %s\n", node->name);
    printf("  add rsp, 8\n");
    printf(".Lend%d:\n", l_label_num);
    printf("  push rax\n");
    return;
  case ND_FUNC_DEF:
    // prologue
    // allocate 26 variables
    printf(".global %s\n", node->name);
    printf("%s:\n", node->name);
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", node->lvar_size_byte);
    if(node->func_args != NULL){
      gen_func_args(node->func_args);
    }
    gen_node_list(node->comp_stmt);
    // epilogue
    // the last evaluation result in rax is the return value
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_ADDR:
    printf("  # addr\n");
    gen_lval(node->lhs);
    return;
  case ND_DEREF:
    printf("  # deref\n");
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }
    
  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch(node->kind){
  case ND_ADD:
    if(node->lhs->type != NULL && node->lhs->type->ty == PTR){
      printf("  imul rdi, 8 # multiple by 8 for int addition\n");
    }else if(node->rhs->type != NULL && node->rhs->type->ty == PTR){
      printf("  imul rax, 8 # multiple by 8 for int addition\n");
    }
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    if(node->lhs->type != NULL && node->lhs->type->ty == PTR){
      printf("  imul rdi, 8 # multiple by 8 for int addition\n");
    }else if(node->rhs->type != NULL && node->rhs->type->ty == PTR){
      printf("  imul rax, 8 # multiple by 8 for int addition\n");
    }
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
  if(node->kind == ND_LVAR){
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
    return;
  }
  if(node->kind == ND_DEREF){
    gen(node->lhs);
    return;
  }
  printf("%d\n",node->kind);
  error("ERROR:Left hand side is NOT left value\n");
}

void gen_node_list(node_list* nl){
  if(nl->data == NULL){
    return;
  }
  gen(nl->data);
  printf("  pop rax\n");
  if(nl->next == NULL){
    return;
  }
  gen_node_list(nl->next);
  return;
}

void gen_func_args(node_list* fa){
  int i = 0;
  while(fa != NULL){
    printf("  mov [rbp-%d], %s\n", fa->data->offset, ARG_REG[i]);
    i++;
    fa = fa->next;
  }
  return;
}



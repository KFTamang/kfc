#include "codegen.h"

void enterNewLoop(LoopType new_loop_type, int new_label_number){
  BreakMarker* new = calloc(sizeof(BreakMarker),1);
  new->type = new_loop_type;
  new->label_number = new_label_number;
  new->prev_marker = bm;
  bm = new;
}

void exitLoop(){
  bm->label_number = bm->prev_marker->label_number;
  bm->type = bm->prev_marker->type;
  bm = bm->prev_marker;
}

void pointer_operation(Node* node){
  if(node->lhs->type == NULL && node->rhs->type == NULL){
    return;
  }
  if(node->lhs->type != NULL){
    switch(node->lhs->type->ty){
      case PTR:
        printf("  imul rdi, %ld # multiple by size size for int addition\n", get_type_size_byte(node->lhs->type->ptr_to));
        break;
      case ARRAY:
        printf("  imul rdi, %ld # multiple by size size for int addition\n", get_type_size_byte(node->lhs->type->ptr_to));
        break;
      default: break;
    }
  }
  if(node->rhs->type != NULL){
    switch(node->rhs->type->ty){
      case PTR:
        printf("  imul rax, %ld # multiple by size size for int addition\n", get_type_size_byte(node->rhs->type->ptr_to));
        break;
      case ARRAY:
        printf("  imul rax, %ld # multiple by size size for int addition\n", get_type_size_byte(node->rhs->type->ptr_to));
        break;
      default: break;
    }
  }
}

void move_rax_rdi_per_type(Type* type){
  switch(type->ty){
  case INT:
  case PTR:
    printf("  mov [rax], rdi # debug1\n");
    break;
  case ARRAY:
    move_rax_rdi_per_type(type->ptr_to);
    break;
  case CHAR:
    printf("  mov [rax], dil # debug2\n");
    break;
  default:
    printf("  mov [rax], rdi #debug3\n");
  }
  return;
}

void move_from_pointer_per_type(char* dest, char* src, Type* type){
  switch(type->ty){
  case INT:
  case PTR:
    printf("  mov %s, QWORD PTR[%s]\n", dest, src);
    break;
  case ARRAY:
    move_from_pointer_per_type(dest, src, type->ptr_to);
    break;
  case CHAR:
    printf("  movzx %s, BYTE PTR[%s]\n", dest, src);
    break;
  case VOID:
    break;
  case ENUM:
    move_from_pointer_per_type(dest, src, new_type(INT, NULL));
    break;
  default:
    error("This type is not supported yet\n");
  }
}

// generate stack machine from the sytax tree
void gen(Node* node){
  int l_label_num = g_label_num;
  char str[VAR_NAME_SIZE];
  int arg_num = 0;
  switch(node->kind){
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_VAR:
    gen_lval(node);
    if(node->type->ty == ARRAY){
	    return;
	  }
    printf("  pop rax\n");
    move_from_pointer_per_type("rax","rax",node->type);
    printf("  push rax\n");
    return;
  case ND_VAR_DEC:
    if(node->is_global){
      return;
    }
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    if(node->lhs->type == NULL){
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;      
    }
    move_rax_rdi_per_type(node->lhs->type);
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
    ++g_label_num;
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
    printf("  push rax\n");
    return;
  case ND_WHILE: // while(A) B
    ++g_label_num;
    enterNewLoop(LT_WHILE, l_label_num);
    printf(".Lbeginwhile%d:\n", l_label_num);
    gen(node->cond); // code for conditional expression i.e. A
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lendwhile%d\n", l_label_num);
    gen(node->then); // code 
    printf("  jmp .Lbeginwhile%d\n", l_label_num); 
    printf(".Lendwhile%d:\n", l_label_num);
    exitLoop();
    return;
  case ND_FOR: // for(A;B;C)D
    ++g_label_num;
    enterNewLoop(LT_FOR, l_label_num);
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
    printf(".Lloopendfor%d:\n", l_label_num);
    if(node->end != NULL){
      gen(node->end); // C
    }
    printf("  jmp .Lbeginfor%d\n", l_label_num); 
    printf(".Lendfor%d:\n",l_label_num);
    exitLoop();
    return;
  case ND_BLOCK:
    gen_node_list(node->comp_stmt);
    return;
  case ND_FUNC:
    ++g_label_num;
    while(node->func_args && (arg_num < MAX_ARG_NUM) ){
      printf("  #evaluate argument %d\n", arg_num+1);
      gen(node->func_args->data);
      node->func_args = node->func_args->next;
      ++arg_num;
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
    printf("  jmp .Lendcall%d\n", l_label_num);
    printf(".Lcall%d:\n", l_label_num);
    printf("  sub rsp, 8\n");
    printf("  mov rax, 0\n");
    printf("  call %s\n", node->name);
    printf("  add rsp, 8\n");
    printf(".Lendcall%d:\n", l_label_num);
    printf("  push rax\n");
    return;
  case ND_FUNC_DEF:
    // prologue
    // allocate 26 variables
    printf(".global %s\n", node->name);
    printf("%s:\n", node->name);
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    // fill local var region with 0xdeadbeef for debug
    printf("  sub rsp, %d\n", round_up_to_8(node->var_size_byte+8));
    for(int i=0;i<round_up_to_8(node->var_size_byte)/8; ++i){
      printf("  movq [rbp-%d], 0xdead\n",i*8+8);
    }
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
    printf("  #%d\n",node->type->ty);
    move_from_pointer_per_type("rax", "rax", node->type);
    printf("  push rax\n");
    return;
  case ND_STRUCT_MEM:
    gen_lval(node);
    printf("  pop rax\n");
    move_from_pointer_per_type("rax", "rax", node->member->type);
    printf("  push rax\n");
    return;
  case ND_EMPTY:
    printf("  push rax\n");
    return;
  case ND_NOT:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    printf("  push rax\n");
    return;
  case ND_SWITCH:
    switch_case(node);
    return;
  case ND_BREAK:
    if(bm->type == LT_NONE){
      error("Break sentence is used outside loop block\n");
    }
    if(bm->type == LT_SWITCH){
      printf("  jmp .Lendswitch%d\n",bm->label_number);
      return;
    }
    if(bm->type == LT_FOR){
      printf("  jmp .Lendfor%d\n", bm->label_number);
      return;
    }
    if(bm->type == LT_WHILE){
      printf("  jmp .Lendwhile%d\n", bm->label_number);
      return;      
    }
    error("ERROR: break control should not reach here!\n");
  case ND_CONTINUE:
    if(bm->type == LT_NONE){
      error("Continue sentence is used outside loop block\n");
    }
    if(bm->type == LT_SWITCH){
      printf("  jmp .Lendswitch%d\n",bm->label_number);
      return;
    }
    if(bm->type == LT_FOR){
      printf("  jmp .Lloopendfor%d\n", bm->label_number);
      return;
    }
    if(bm->type == LT_WHILE){
      printf("  jmp .Lbeginwhile%d\n", bm->label_number);
      return;      
    }
    error("ERROR: Continue control should not reach here!\n");
  }
    
  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch(node->kind){
  case ND_ADD:
    pointer_operation(node);
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    pointer_operation(node);
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_MOD:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    printf("  mov rax, rdx\n");
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
  case ND_AND:
    printf("  and rax, rdi\n");
    break;
  case ND_OR:
    printf("  or rax, rdi\n");
    break;
  }
  printf("  push rax\n");

}

void gen_lval(Node* node){
  if(node->kind == ND_VAR){
    if(node->is_global){
      printf("  lea rax, %s[rip]\n", node->name);
      printf("  push rax\n");
      return;
    }

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
    return;
  }
  if(node->kind == ND_DEREF){
    gen(node->lhs);
    return;
  }
  if(node->kind == ND_STRUCT_MEM){
    gen_lval(node->lhs);
    printf("  pop rax\n");
    printf("  add rax, %d\n", node->member->offset);
    printf("  push rax\n");
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
    ++i;
    fa = fa->next;
  }
  return;
}

void gen_global_var(Scope* global_scope){
  Var* v = global_scope->var;
  if(v==NULL){
    printf("#global var empty\n");
  }
  while(v!=NULL){
    if(v->type->is_extern){
      // nop
    }else{
      printf("  .global %s\n", v->name);
      printf("%s:\n",v->name);
      printf("  .zero %d\n",v->size_byte);
    }
    v = v->next;
  }
  return;
}

void switch_case(Node* node){
  gen_lval(node->lhs);
  printf("  pop rax\n");
  printf("  mov rax, [rax]\n");
  int l_label_num = g_label_num;
  int l_label_switchend = g_label_num;
  // local variables for default case
  node_list* default_node_list = NULL;
  enterNewLoop(LT_SWITCH, l_label_switchend);
  for(Switch_list* sw=node->sw_l; sw; sw=sw->next){
    if(sw->is_default){
      if(default_node_list){
        error("Multiple defalut case in switch-case block\n");
      }
      default_node_list = sw->nl;
      break;
    }
    printf("  cmp rax, %d\n", sw->case_num);
    printf("  je .Lswitchcase%d\n", l_label_num);
    ++l_label_num;
    ++g_label_num;
  }
  printf("  jmp .Lswitchcase%d\n", l_label_num);
  l_label_num = l_label_switchend;
  for(Switch_list* sw=node->sw_l; sw; sw=sw->next){
    if(sw->is_default){
      break;
    }
    printf("  .Lswitchcase%d:\n", l_label_num);
    gen_node_list(sw->nl);
    ++l_label_num;
    ++g_label_num;
  }
  printf("  .Lswitchcase%d:\n", l_label_num);
  if(default_node_list){
    gen_node_list(default_node_list);
  }
  printf("  .Lendswitch%d:\n", l_label_switchend);
  exitLoop();
}

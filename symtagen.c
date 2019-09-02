#include "kfc.h"

// generate hash from node
unsigned int hash_nodename(char* node_name){
  unsigned int hash = 0;
  int len = strlen(node_name);
  for(int i=0;i<len;i++){
    hash += node_name[i] << i*7;
  }
  hash = hash % MAX_SYMBOL_NUM;
  return hash;
}


int get_lvar_number_in_scope(char* scope_name){
  //  printf("scope:%d,%s", scope->kind, scope->name);
  int i = 0;
  int scope_hash = hash_nodename(scope_name);

  while(sym_table[scope_hash+i] != NULL){
    i++;
  }
  return i-1;
}

void register_func_def(char* func_name){
  //  printf("scope:%d,%s\n", scope->kind, scope->name);
  //  printf("node:%d,%s\n", node->kind, node->name);
  //  printf("hash:%d\n", hash_node(node));
  //  int lvar_num = get_lvar_number_in_scope(func_name);
  int scope_hash = hash_nodename(func_name);
  SymbolKind sk = SY_FUNC;
  symbol* sym = calloc(1, sizeof(symbol));
  //  char* name = calloc(1, sizeof(char)*(strlen(func_name)+1));
  sym->kind = sk;
  sym->name = func_name;
  sym->scope = func_name;
  sym->size = 0;
  sym_table[scope_hash] = sym;
  return;
}

void register_lvar(char* scope_name, char* node_name){
  //  printf("scope:%d,%s\n", scope->kind, scope->name);
  //  printf("node:%d,%s\n", node->kind, node->name);
  //  printf("hash:%d\n", hash_node(node));
  int lvar_num = get_lvar_number_in_scope(scope_name);
  unsigned int scope_hash = hash_nodename(scope_name);
  SymbolKind sk = SY_LVAR;
  symbol* sym = calloc(1, sizeof(symbol));
  char* name = calloc(1, sizeof(char)*(strlen(node_name)+1));
  sym->kind = sk;
  sym->name = node_name;
  sym->size = 4;
  sym->addr = 4*lvar_num;
  sym->scope = scope_name;
  sym_table[scope_hash + lvar_num + 1] = sym; // plus one to avoid function itself
  sym_table[scope_hash]->size += sym->size; // increase function's local var size
  return;
}

int get_ordinal_number_in_scope(char* scope_name, char* lvar_name){
  unsigned int scope_hash = hash_nodename(scope_name);
  symbol* sym = sym_table[scope_hash];
  int i = 0;
  while(sym != NULL){
    if(strncmp(sym->name, lvar_name, strlen(lvar_name))==0){
      return i;
    }
    i++;
    sym = sym_table[scope_hash+i];
  }
  return -1;
}

void symtabgen(Node* node, Node* scope){
  if(node == NULL){
    return;
  }
  node_list* nl;
  char str[VAR_NAME_SIZE];
  int order;
  switch(node->kind){
  case ND_LVAR:
    //    strncpy(str, node->name, node->len);
    //    str[node->len] = '\0';
    order = get_ordinal_number_in_scope(scope->name, node->name);
    if(order > 0){
      node->offset = order;
      return;
    }
    node->offset = 4 * get_lvar_number_in_scope(scope->name);
    register_lvar(scope->name, node->name);
    return;
  case ND_FUNC:
    return;
  case ND_FUNC_DEF:
    printf("func name : %s\n",node->name);
    register_func_def(node->name);
    nl = node->comp_stmt;
    while(nl != NULL){
      symtabgen(nl->data, node);
      nl = nl->next;
    }
    return;

  // two term nodes
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
  case ND_EQU:
  case ND_NEQ:
  case ND_LWT:
  case ND_LEQ:
  case ND_NUM:
  case ND_ASSIGN:
  case ND_RETURN:
    symtabgen(node->lhs, scope);
    symtabgen(node->rhs, scope);
    return;

  case ND_IF:
    symtabgen(node->cond, scope);
    symtabgen(node->then, scope);
    symtabgen(node->els, scope);
    return;
  case ND_WHILE:
    symtabgen(node->cond, scope);
    symtabgen(node->then, scope);
    return;
  case ND_FOR:
    symtabgen(node->init, scope);
    symtabgen(node->cond, scope);
    symtabgen(node->end, scope);
    symtabgen(node->then, scope);
    return;
  case ND_BLOCK:
    nl = node->comp_stmt;
    while(nl != NULL){
      symtabgen(nl->data, scope);
      nl = node->comp_stmt->next;
    }
    return;
  }
}

int get_lvar_address(char* scope_name, char* name){
  int scope_hash = hash_nodename(scope_name);
  int i = 0;
  int len = strlen(name);
  symbol* sym = sym_table[scope_hash];
  while(sym != NULL){
    if(strncmp(name,sym->name,len) == 0){
      return sym->addr;
    }
    i++;
    sym = sym_table[scope_hash+i];
  }
  return -1;
}

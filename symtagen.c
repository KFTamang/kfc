#include "kfc.h"

// generate hash from node
unsigned int hash_node(Node* node){
  unsigned int hash = ((unsigned int)node) % MAX_SYMBOL_NUM; // temporary use address
  return hash;
}

int get_lvar_number_in_scope(Node* scope){
  printf("scope:%d,%s", scope->kind, scope->name);
  int i = 0;
  int scope_hash = hash_node(scope);

  while(sym_table[scope_hash+i] != NULL){
    i++;
  }
  return i;
}

void append_lvar(Node* scope, Node* node){
  printf("scope:%d,%s\n", scope->kind, scope->name);
  printf("node:%d,%s\n", node->kind, node->name);
  printf("hash:%d\n", hash_node(node));
  int lvar_num = get_lvar_number_in_scope(scope);
  int scope_hash = hash_node(scope);
  SymbolKind sk;
  switch(node->kind){
  case ND_LVAR:
    sk = SY_LVAR;
    break;
  case ND_FUNC_DEF:
    sk = SY_FUNC;
    break;
  default:
    sk = SY_GVAR;
    break;
  }
  symbol* sym = calloc(1, sizeof(symbol));
  char* name = calloc(1, sizeof(char)*(node->len+1));
  strncpy(name, node->name, node->len);
  name[node->len] = '\0';
  sym->kind = sk;
  sym->name = name;
  sym->size = 4;
  sym->addr = 4*lvar_num;
  sym->scope = scope;
  sym_table[scope_hash + lvar_num] = sym;
  return;
}

void symtabgen(Node* node, Node* scope){
  if(node == NULL){
    return;
  }
  node_list* nl;
  char str[VAR_NAME_SIZE];
  switch(node->kind){
  case ND_LVAR:
    strncpy(str, node->name, node->len);
    str[node->len] = '\0';
    append_lvar(scope, node);
    return;
  case ND_FUNC:
    return;
  case ND_FUNC_DEF:
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
    while(nl->data != NULL){
      symtabgen(nl->data, node);
      nl = node->comp_stmt->next;
    }
    return;
  }
}

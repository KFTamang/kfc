<<<<<<< HEAD
#include "khc.h"
=======
#include "kfc.h"
>>>>>>> WIP for func definition

// generate hash from node
int hash_node(Node* node){
  int hash = ((int)node) % MAX_SYMBOL_NUM; // temporary use address
  return hash;
}

Node* is_in_scope(Node* scope){
  return scope;
}

void append_lvar(Node* scope, Node* node){
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
    if(!is_in_scope(scope)){
      append_lvar(scope, node);
    }
    return;
  case ND_FUNC:
    return;
  case ND_FUNC_DEF:
    nl = node->comp_stmt;
    while(nl != NULL){
      symtabgen(nl->data, node);
      nl = node->comp_stmt->next;
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

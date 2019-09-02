#include "khc.h"

// generate hash from node
int hash_node(Node* node){
  int hash = ((int)node) % MAX_SYMBOL_NUM; // temporary use address
  return hash;
}

Node* is_in_scope(Node* scope){

}

void append_lvar(Node* scope, Node* node){}

void symtabgen(Node* node, Node* scope){
  char str[VAR_NAME_SIZE];
  switch(node->kind){
  case ND_LVAR:
    strncpy(str, node->name, node->len);
    str[node->len] = '\0';
    if(!is_in_scope(scope)){
      append_lvar(scope, node);
    }
  }
  return;
  case ND_FUNC
}

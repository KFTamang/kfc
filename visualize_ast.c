#include "kfc.h"

void tree_print(Node* node, int i){
  char str[VAR_NAME_SIZE];
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
  case ND_BLOCK:
    printf("block\n");
    while(node->comp_stmt!=NULL){
      tree_print(node->comp_stmt->data, i+1);
      node->comp_stmt = node->comp_stmt->next;
    }
    return;
  case ND_FUNC:
    if(node->len >= VAR_NAME_SIZE){
      error("Too long name of variable");
      return;
    }
    strncpy(str, node->name, node->len);
    str[node->len] = '\0';
    printf("func %s\n",str);
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



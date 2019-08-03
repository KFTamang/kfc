#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token{
  TokenKind kind;
  Token *next;
  int val;
  char *str;
};

// curent token in interest
Token *token;
char *user_input;

// error messager
void error(char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

void error_at(char *loc, char *fmt, ...){
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool consume(char op){
  if (token->kind != TK_RESERVED || token->str[0] != op){
    return false;
  }
  token = token->next;
  return true;
}

void expect(char op){
  if (token->kind != TK_RESERVED || token->str[0] != op){
    error_at(token->str, "Not %s", op);
  }
  token = token->next;
}

int expect_number(){
  if (token->kind != TK_NUM){
    error_at(token->str, "Not a number");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof(){
  return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str){
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token *tokenize(char *p){
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while(*p){
    if (isspace(*p)){
      p++;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'){
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }
    if (isdigit(*p)){
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(cur->str, "Failed tokenizing\n");
  }

  new_token(TK_EOF, cur, p);
  return head.next;

}

// node kind of abstract syntax tree
typedef enum{
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // number
} NodeKind;

typedef struct Node Node;

// abstract syntax tree
struct Node {
  NodeKind kind; // node kind
  Node *lhs;     // pointer to left hand side
  Node *rhs;     // pointer to right hand side
  int val;       // value if kind is ND_NUM
};

// node generator if not number
Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs  = lhs;
  node->rhs  = rhs;
  return node;
}

// node generator if number (meaning end node)
Node *new_node_num(int val){
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val  = val;
  return node;
}

Node* num();
Node* expr();
Node* mul();

// ENBF terminal symbol for number
Node *num(){
  return new_node_num(expect_number());
}

//ENBF expr = mul ( "+" mul | "-" mul )*
Node *expr(){
  Node *node = mul();
  //  printf("%d",node->val); // for debug
  
  for(;;){
    if(consume('+')){
      //      printf("+"); // for debug
      node = new_node(ND_ADD, node, mul());
    }else if(consume('-')){
      //      printf("-"); // for debug
      node = new_node(ND_SUB, node, mul());
    }else{
      return node;
    }
  }
}

// ENBF mul = num ( "*" num | "/" num )*
Node *mul(){
  Node *node = num();

  for(;;){
    if(consume('*')){
      node = new_node(ND_MUL, node, num());
    }else if(consume('/')){
      node = new_node(ND_DIV, node, num());
    }else{
      return node;
    }
  }
}

/* // ENBF term = ( expr ) | num */
/* Node *term(){ */
/*   if(consume('(')){ */
/*     Node* node = expr(); */
/*     expect(')'); */
/*     return node; */
/*   } */
/*   return new_node_num(expect_number()); */
/* } */


// generate stack machine from the sytax tree
// only print syntax tree
// TODO: implement stack machine
void gen(Node* node){
  if(node->kind == ND_NUM){
    printf("  push %d\n", node->val);
    //    printf("%d\n",node->val); // for debug
    return;
  }
    
  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch(node->kind){
  case ND_ADD:
    printf("  add rax, rdi\n");
    //    printf("+\n"); // for debug
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    //    printf("-\n"); // for debug
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  }
  printf("  push rax\n");

}

int main(int argc, char **argv){
  if (argc != 2){
    fprintf(stderr, "wrong number of arguments\n");
  return 1;
  }

  // tokenize and parse
  user_input = argv[1];
  token = tokenize(user_input);
  Node* node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // climbing down the tree and generate code
  gen(node);
  
  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}

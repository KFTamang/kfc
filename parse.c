#include "9cc.h"

bool consume(char* op){
  if(token->kind != TK_RESERVED ||
     strlen(op) != token->len   ||
     memcmp(token->str, op, token->len) ){
    return false;
  }
  token = token->next;
  return true;
}

void expect(char* op){
  if(token->kind != TK_RESERVED ||
     strlen(op) != token->len   ||
     memcmp(token->str, op, token->len) ){
    error_at(token->str, "Not %s", op);
  }
  token = token->next;
}

int expect_number(){
  if(token->kind != TK_NUM){
    error_at(token->str, "Not a number");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof(){
  return token->kind == TK_EOF;
}

Token* new_token(TokenKind kind, Token* cur, char* str, int len){
  Token* tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

Token* tokenize(char* p){
  Token head;
  head.next = NULL;
  Token* cur = &head;

  while(*p){
    if (isspace(*p)){
      p++;
      continue;
    }
    if(strncmp(p,"==",2) == 0 || strncmp(p,">=",2) == 0 ||
       strncmp(p,"<=",2) == 0 || strncmp(p,"!=",2) == 0 ){
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'||
	*p == '<' || *p == '>'){
      cur = new_token(TK_RESERVED, cur, p, 1);
      p++;
      continue;
    }
    if (isdigit(*p)){
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(cur->str, "Failed tokenizing\n");
  }

  new_token(TK_EOF, cur, p, 1);
  return head.next;

}

// node generator if not number
Node* new_node(NodeKind kind, Node* lhs, Node* rhs){
  Node* node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs  = lhs;
  node->rhs  = rhs;
  return node;
}

// node generator if number (meaning end node)
Node* new_node_num(int val){
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val  = val;
  return node;
}

// ENBF terminal symbol for number
Node* num(){
  return new_node_num(expect_number());
}

//ENBF expr = equality
Node* expr(){
  return equality();
}

// ENBF equality = relational ( "==" relational | "!=" relatinal ) *
Node* equality(){
  Node* node = relational();

  for(;;){
    if(consume("==")){
      node = new_node(ND_EQU, node, relational());
    }else if(consume("!=")){
      node = new_node(ND_NEQ, node, relational());
    }else{
      return node;
    }
  }
  
}

// ENBF relational = add ("<" add | "<=" add | ">" add | ">=" add) *
Node* relational(){
  Node* node = add();
  
  for(;;){
    if(consume("<")){
      node = new_node(ND_LWT, node, add());
    }else if(consume("<=")){
      node = new_node(ND_LEQ, node, add());
    }else if(consume(">")){
      node = new_node(ND_LWT, add(), node);
    }else if(consume(">=")){
      node = new_node(ND_LEQ, add(), node);
    }else{
      return node;
    }
  }
}

// ENBF add = mul ( "+" mul | "-" mul )*
Node* add(){
  Node* node = mul();
  
  for(;;){
    if(consume("+")){
      node = new_node(ND_ADD, node, mul());
    }else if(consume("-")){
      node = new_node(ND_SUB, node, mul());
    }else{
      return node;
    }
  }
}

// ENBF mul = unary ( "*" unary | "/" unary )*
Node* mul(){
  Node* node = unary();

  for(;;){
    if(consume("*")){
      node = new_node(ND_MUL, node, unary());
    }else if(consume("/")){
      node = new_node(ND_DIV, node, unary());
    }else{
      return node;
    }
  }
}

// ENBF term = ( expr ) | num
Node* term(){
  if(consume("(")){
    Node* node = expr();
    expect(")");
    return node;
  }
  return num();
}

// ENBF unary = ("+" | "-")? term
Node* unary(){
  if(consume("+")){
    return term();
  }
  if(consume("-")){
    return new_node(ND_SUB,new_node_num(0), term());
  }
  return term();
}


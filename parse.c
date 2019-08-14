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

bool consumeByKind(TokenKind tkind){
  if(token->kind == tkind){
    token = token->next;
    return true;
  }
  return false;
}

int is_alnum(char c){
  return (('a' <= c && c <= 'z') ||
	  ('A' <= c && c <= 'Z') ||
	  ('0' <= c && c <= '9') ||
	  ( c == '_') );
}

Token* consume_ident(){
  Token* ret = NULL;
  if(token->kind == TK_IDENT ){
    ret = token;
    token = token->next;
  }
  return ret;
}

void expect(char* op){
  if(token->kind != TK_RESERVED ||
     strlen(op) != token->len   ||
     memcmp(token->str, op, token->len) ){
    error_at(token->str, "Not %s", op);
  }
  token = token->next;
}

void expect_in_future(char* op){
  if(token->next->kind != TK_RESERVED ||
     strlen(op) != token->next->len   ||
     memcmp(token->next->str, op, token->next->len) ){
    error_at(token->next->str, "Not %s", op);
  }
  token->next = token->next->next;
}

bool is_next_symbol(char* op){
  if(token->next->kind != TK_RESERVED ||
     strlen(op) != token->next->len   ||
     memcmp(token->next->str, op, token->next->len)){
    return false;
  }
  return true;
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
	*p == '<' || *p == '>' || *p == ';' || *p == '=' || *p == '{' || *p == '}'){
      cur = new_token(TK_RESERVED, cur, p, 1);
      p++;
      continue;
    }
    if (isdigit(*p)){
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    if (is_alnum(*p)){
      int i = 1;
      while(is_alnum(*(p+i))){
	++i;
      }
      // if "if"
      if(i==2 && strncmp(p, "if", i)==0){ 
	cur = new_token(TK_IF, cur, p, i);
	p += i;
	continue;
      }
      // if else
      if(i==4 && strncmp(p, "else", i)==0){ 
	cur = new_token(TK_ELSE, cur, p, i);
	p += i;
	continue;
      }
      // if return
      if(i==6 && strncmp(p, "return", i)==0){ 
	cur = new_token(TK_RETURN, cur, p, i);
	p += i;
	continue;
      }
      // if while
      if(i==5 && strncmp(p, "while", i)==0){ 
	cur = new_token(TK_WHILE, cur, p, i);
	p += i;
	continue;
      }
      // if for
      if(i==3 && strncmp(p, "for", i)==0){ 
	cur = new_token(TK_FOR, cur, p, i);
	p += i;
	continue;
      }
      // local var
      cur = new_token(TK_IDENT, cur, p, i);
      p += i;
      continue;
    }

    error_at(cur->str, "Failed tokenizing\n");
  }

  new_token(TK_EOF, cur, p, 1);
  return head.next;

}

node_list* append_node_list(node_list* current, Node* data){
  node_list* new_nl = calloc(1, sizeof(node_list));
  new_nl->data = data;
  new_nl->next = NULL;
  current->next = new_nl;
  return new_nl;
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

// search for local variable from var chain
LVar* find_lvar(Token* tok){
  for(LVar* var=locals; var; var=var->next){
    if(var->len == tok->len && !memcmp(tok->str, var->name, var->len)){
      return var;
    }
  }
  return NULL;
}

// ENBF program = stmt*
void program(){
  int i = 0;
  while(!at_eof()){
    code[i] = stmt();
    ++i;
  }
  code[i] = NULL;
}

// ENBF stmt = expr ";" | "return" expr ";"
//           | "{" stmt* "}"
//           | "if" "(" expr ")" stmt ("else" stmt)?
//           | "while" "(" expr ")" stmt
//           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
Node* stmt(){
  Node* node;
  // if return statement
  if(consumeByKind(TK_RETURN)){
    node = new_node(ND_RETURN, expr(), NULL);
    expect(";");
  }else if(consume("{")){ // compound statement (block) {}
    node = new_node(ND_BLOCK, NULL, NULL);
    node_list* comp_stmt = calloc(1, sizeof(node_list));
    node->comp_stmt = comp_stmt;
    comp_stmt->next = NULL;
    if(consume("}")){
      comp_stmt->data = NULL;
    }else{
      comp_stmt->data = stmt();
      while(!consume("}")){
	comp_stmt = append_node_list(comp_stmt, stmt());
      }
    }
  }else if(consumeByKind(TK_IF)){ // "if" statement
    expect("(");
    Node* cond = expr();
    expect(")");
    Node* then = stmt();
    node = new_node(ND_IF, NULL, NULL);
    node->cond = cond;
    node->then = then;
    if(consumeByKind(TK_ELSE)){
      Node* els = stmt();
      node->els = els;
    }else{
      node->els = NULL;
    }
  }else if(consumeByKind(TK_WHILE)){ // while loop
    expect("(");
    Node* cond = expr();
    expect(")");
    Node* then = stmt();
    node = new_node(ND_WHILE, NULL, NULL);
    node->cond = cond;
    node->then = then;
  }else if(consumeByKind(TK_FOR)){ // for loop
    expect("(");
    Node* init;
    Node* cond;
    Node* end;
    //    if(is_next_symbol(";")){ // empty initialize section
    if(consume(";")){
      init = NULL;
    }else{ // initialize section
      init = expr();
      expect(";");      
    }
    //    if(is_next_symbol(";")){ // empty condition section
    if(consume(";")){
      cond = NULL;
    }else{ // initialize section
      cond = expr();
      //      consume(";");
      expect(";");      
    }
    //    if(is_next_symbol(")")){ // empty initialize section
    if(consume(")")){
      end = NULL;
    }else{ // initialize section
      end = expr();
      expect(")");
    }
    Node* then = stmt();
    node = new_node(ND_FOR, NULL, NULL);
    node->init = init;
    node->cond = cond;
    node->end = end;
    node->then = then;
  }else{
    node = expr();
    expect(";");
  }
  return node;
}

// ENBF expr = assign
Node* expr(){
  return assign();
}

// ENBF assign = equality ("=" assign)?
Node* assign(){
  Node* node = equality();
  if (consume("=")){
    node = new_node(ND_ASSIGN, node, assign());
  }
  return node;
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

// ENBF term = ( expr ) | num | ident ( "(" ")" )?
Node* term(){
  if(consume("(")){
    Node* node = expr();
    expect(")");
    return node;
  }
  Token* tok = consume_ident();
  if(tok){
    Node* node;
    if(consume("(")){ // function
      expect(")");
      node = calloc(1, sizeof(Node));
      node->kind = ND_FUNC;
      node->name = tok->str;
      node->len = tok->len;
    }else{ // variable
      node = calloc(1, sizeof(Node));
      node->kind = ND_LVAR;
      LVar* var = find_lvar(tok);
      if(var){
	node->offset = var->offset;
      }else{
	if(locals){
	  LVar* new_var = calloc(1, sizeof(LVar));
	  new_var->next = locals;
	  new_var->name = tok->str;
	  new_var->len = tok->len;
	  new_var->offset = locals->offset + 8;
	  node->offset = new_var->offset;
	  locals = new_var;
	}else{
	  locals = calloc(1, sizeof(LVar));
	  locals->next = NULL;
	  locals->name = tok->str;
	  locals->len = tok->len;
	  locals->offset = 0;
	}
      }
    }
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

// ENBF terminal symbol for number
Node* num(){
  return new_node_num(expect_number());
}

#include "kfc.h"


node_list* new_node_list(Node* node){
  node_list* nl = calloc(1, sizeof(node_list));
  nl->next = NULL;
  nl->data = node;
  return nl;
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

// search for local variable in the scope from var chain
LVar* find_lvar(Token* tok){
  for(LVar* var=locals; var; var=var->next){
    if(var->len == tok->len && !memcmp(tok->str, var->name, var->len) &&
       !strncmp(var->scope_name, g_current_scope, strlen(var->scope_name))){
      return var;
    }
  }
  return NULL;
}

void append_lvar(Token* tok){
  if(locals){
    LVar* new_var = calloc(1, sizeof(LVar));
    new_var->next = locals;
    new_var->name = tok->str;
    new_var->len = tok->len;
    new_var->size_byte = 16;
    new_var->offset = get_lvar_size_byte() + 8;
    new_var->scope_name = g_current_scope;
    locals = new_var;
  }else{
    locals = calloc(1, sizeof(LVar));
    locals->next = NULL;
    locals->name = tok->str;
    locals->len = tok->len;
    locals->size_byte = 8;
    locals->offset = 8;
    locals->scope_name = g_current_scope;
  }
}

int get_lvar_size_byte(){
  int lvar_size = 0;
  for(LVar* var=locals; var; var=var->next){
    if(!strncmp(var->scope_name, g_current_scope, strlen(var->scope_name))){
      lvar_size += var->size_byte;
    }
  }
  return lvar_size;
}

// ENBF program = func_def*
void program(){
  int i = 0;
  while(!at_eof()){
    code[i] = func_def();
    ++i;
  }
  code[i] = NULL;
}

// ENBF func_def = ident "(" lvar_dec? ")" "{" stmt* "}"
Node* func_def(){
  Node* node;
  Token* tok = consume_ident();
  if(tok == NULL){
    return NULL;
  }
  // name
  node = new_node(ND_FUNC_DEF, NULL, NULL);
  char* node_name = calloc(1, sizeof(char)*(tok->len+1));
  strncpy(node_name, tok->str, tok->len);
  node->name = node_name;
  node->len = tok->len;
  g_current_scope = node_name;
  // arguments
  expect("(");
  if(consume(")")){ // no argument
    node->func_args = NULL;
  }else{ // one argument
    Node* arg_lvar = lvar_dec();
    //    Token* arg = consume_ident();
    if(!arg_lvar){
      error_at(token->str, "Argument variables are wrong\n");
    }
    node->func_args = new_node_list(arg_lvar);
    node_list* next = node->func_args;
    while(consume(",")){
      arg_lvar = lvar_dec();
      next = append_node_list(next, arg_lvar);
    }
    expect(")");
  }
  // statements
  expect("{");
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
  node->lvar_size_byte = get_lvar_size_byte();
  return node;
}

// ENBF stmt = expr ";" | "return" expr ";"
//           | "{" stmt* "}"
//           | "if" "(" expr ")" stmt ("else" stmt)?
//           | "while" "(" expr ")" stmt
//           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//           | lvar_dec ";"
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
    node = lvar_dec();
    if(node != NULL){
      expect(";");
      return node;
    }
    node = expr();
    expect(";");
  }
  return node;
}

// ENBF lvar_dec = "int" lvar
Node* lvar_dec(){
  if(!consumeByKind(TK_TYPE_INT)){
    return NULL;
  }
  Token* tok = consume_ident();
  if(tok == NULL){
    return NULL;
  }
  LVar* var = find_lvar(tok);
  // if variable is already declared
  if(var != NULL){
    error_at(token->str,"Local variable %s is already defined\n", var->name);
    exit(1);
  }
  append_lvar(tok);
  var = find_lvar(tok);
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR_DEC;
  char* node_name = calloc(1, sizeof(tok->len+1));
  strncpy(node_name, tok->str, tok->len);
  node_name[tok->len+1] = '\0';
  node->name = node_name;
  node->len = tok->len;
  node->offset = var->offset;
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

// ENBF term = ( expr ) | num | ident
Node* term(){
  if(consume("(")){
    Node* node = expr();
    expect(")");
    return node;
  }
  if(is_kind(TK_IDENT)){
    Node* node = ident();
    return node;
  }
  return num();
}

// ENBF ident = lvar | func
Node* ident(){
  Token* tok = consume_ident();
  Node* node;
  if(is_symbol("(")){
    node = func(tok);
  }else{ // variable
    node = lvar(tok);
  }
  return node;
}

// ENBF lvar
Node* lvar(Token* tok){
  char* node_name = calloc(1, sizeof(tok->len+1));
  strncpy(node_name, tok->str, tok->len);
  node_name[tok->len+1] = '\0';
  LVar* var = find_lvar(tok);
  if(var == NULL){
    error_at(token->str, "Variable %s is not declared\n", node_name);
    exit(1);
  }

  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->name = node_name;
  node->len = tok->len;
  node->offset = var->offset;

  return node;
}

// ENBF func = ident ( "(" (expr (, expr)*)? ")" )
Node* func(Token* tok){
  Node* node;
  node = calloc(1, sizeof(Node));
  node->kind = ND_FUNC;
  char* node_name = calloc(1, sizeof(char)*(tok->len+1));
  strncpy(node_name, tok->str, tok->len);
  node->name = node_name;
  node->len = tok->len;
  expect("(");
  if(consume(")")){ // no argument
    node->func_args = NULL;
  }else{ // one argument
    node->func_args = new_node_list(expr());
    node_list* next = node->func_args;
    while(consume(",")){
      next = append_node_list(next, expr());
    }
    expect(")");
  }
  return node;
}

// ENBF unary = "+"? term
//            | "-"  term
//            | "&"  unary
//            | "*"  unary
Node* unary(){
  if(consume("+")){
    return term();
  }
  if(consume("-")){
    return new_node(ND_SUB,new_node_num(0), term());
  }
  if(consume("&")){
    return new_node(ND_ADDR, unary(), NULL);
  }
  if(consume("*")){
    return new_node(ND_DEREF, unary(), NULL);
  }
  return term();
}

// ENBF terminal symbol for number
Node* num(){
  return new_node_num(expect_number());
}

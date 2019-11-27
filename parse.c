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

Scope* gen_new_scope(Scope* parent, ScopeKind sk){
  Scope* scope = calloc(1, sizeof(Scope));
  scope->parent = parent;
  scope->sk = sk;
  return scope;
}

// node generator if not number
Node* new_node(NodeKind kind, Node* lhs, Node* rhs){
  Node* node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs  = lhs;
  node->rhs  = rhs;
  switch(kind){
  case ND_NUM:
	node->type = &g_type_int;
	break;
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
  case ND_EQU:
  case ND_NEQ:
  case ND_LWT:
  case ND_LEQ:
  case ND_ASSIGN:
	if((lhs->type != NULL && lhs->type->ty == PTR) || 
	   (rhs->type != NULL && rhs->type->ty == PTR) ){
	  node->type = &g_type_ptr;
	}else{
	  node->type = &g_type_int;
	}
	break;
  case ND_ADDR:
	node->type = &g_type_ptr;
	break;
  default:
	break;
  }
  return node;
}

// node generator if number (meaning end node)
Node* new_node_num(int val){
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val  = val;
  return node;
}

// type generator
Type* new_type(TY ty, Type* ptr_to){
  Type* type = calloc(sizeof(Type),1);
  type->ty = ty;
  type->ptr_to = ptr_to;
  return type;
}

Var* find_var_in_scope(Token* tok, Scope* scope){
  if(scope==NULL){
	return NULL;
  }
  for(Var* var=scope->var; var; var=var->next){
    if(var->len == tok->len && !memcmp(tok->str, var->name, var->len)){
      return var;
    }
  }
  return NULL;
}

  
Var* find_var_in_function_scope(Token* tok, Scope* scope){
  if(scope==NULL){
	return NULL;
  }
  Var* var = find_var_in_scope(tok, scope);
  if(var!=NULL){
	return var;
  }
  if(scope->sk==GLOBAL || scope->parent==NULL){
	return NULL;
  }
  return find_var_recursively(tok, scope->parent);
}

Var* find_var_recursively(Token* tok, Scope* scope){
  if(scope==NULL){
	return NULL;
  }
  Var* var = find_var_in_scope(tok, scope);
  if(var!=NULL){
	return var;
  }
  if(scope->parent==NULL){
	return NULL;
  }
  return find_var_recursively(tok, scope->parent);
}

void append_var_to_scope(Token* tok, Type* type, Scope* scope){
  Var* new_var = calloc(1, sizeof(Var));
  new_var->next = scope->var;
  char* var_name = calloc(1, tok->len+1);
  strncpy(var_name, tok->str, tok->len);
  var_name[tok->len] = '\0';
  new_var->name = var_name;
  new_var->len = tok->len;
  new_var->size_byte = get_type_size_byte(type);
  new_var->offset = get_var_size_byte(g_current_scope) + 8;
  // offset for array is one unit element size lower from the total size
  if(type->ty == ARRAY){ 
	new_var->offset += get_type_size_byte(type) - get_type_size_byte(type->ptr_to);
  }
  scope->var = new_var;
}

size_t get_type_size_byte(Type* type){
  if(type == NULL){
	error("Type is not specified\n");
	return 0;
  }
  switch(type->ty){
  case INT:
	return 8;
  case PTR:
	return 8;
  case ARRAY:
	return type->array_size * get_type_size_byte(type->ptr_to);
  default:
	error("Type is not supported\n");
	return 0;
  }
}

size_t get_var_size_byte(Scope* scope){
  if(scope==NULL){
	return 0;
  }
  int var_size = 0;
  for(Var* var=scope->var; var; var=var->next){
	var_size += var->size_byte;
  }
  if(scope->sk == BLOCK){
	var_size += get_var_size_byte(scope->parent);;
  }	
  return var_size;
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

// ENBF func_def = "int" ident "(" var_dec? ")" "{" stmt* "}"
Node* func_def(){
  Node* node;
  if(!consumeByKind(TK_TYPE_INT)){
    error_at(token->str, "Function definition must follow type defition");
  }
  Token* tok = consume_ident();
  if(tok == NULL){
    return NULL;
  }
  // name
  node = new_node(ND_FUNC_DEF, NULL, NULL);
  char* node_name = calloc(1, tok->len+1);
  strncpy(node_name, tok->str, tok->len);
  node->name = node_name;
  node->len = tok->len;
  // arguments
  expect("(");
  g_current_scope = gen_new_scope(g_global_scope, LOCAL);
  if(consume(")")){ // no argument
    node->func_args = NULL;
  }else{ // one argument
    Node* arg_var = var_dec();
    //    Token* arg = consume_ident();
    if(!arg_var){
      error_at(token->str, "Argument variables are wrong\n");
    }
    node->func_args = new_node_list(arg_var);
    node_list* next = node->func_args;
    while(consume(",")){
      arg_var = var_dec();
      next = append_node_list(next, arg_var);
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
  node->var_size_byte = get_var_size_byte(g_current_scope);
  return node;
}

// ENBF stmt = expr ";" | "return" expr ";"
//           | "{" stmt* "}"
//           | "if" "(" expr ")" stmt ("else" stmt)?
//           | "while" "(" expr ")" stmt
//           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//           | var_dec ";"
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
    node = var_dec();
    if(node != NULL){
      expect(";");
      return node;
    }
    node = expr();
    expect(";");
  }
  return node;
}

// ENBF type_def = "int" "*"*
Type* type_def(){
  if(!consumeByKind(TK_TYPE_INT)){
    return NULL;
  }
  Type* type = new_type(INT, NULL);
  while(consume("*")){
    type = new_type(PTR, type);
  }
  return type;
}

Type* new_array_type(Type* base, size_t size){
  Type* new_array = calloc(1, sizeof(Type));
  new_array->ty = ARRAY;
  new_array->ptr_to = base;
  new_array->array_size = size;
  return new_array;
}

// ENBF var_dec = type_def var ("[" num "]")?
Node* var_dec(){
  Type* this_type = type_def();
  if(this_type == NULL){
    return NULL;
  }
  Token* tok = consume_ident();
  if(tok == NULL){
    return NULL;
  }
  return new_var_node(this_type, tok);
}

Node* new_var_node(Type* type, Token* tok){
  Var* var = find_var_in_scope(tok, g_current_scope);
  // if variable is already declared
  if(var != NULL){
    error_at(token->str,"Local variable %s is already defined\n", var->name);
    exit(1);
  }
  // if array
  if(consume("[")){
	type = new_array_type(type, expect_number());
	expect("]");
  }
  append_var_to_scope(tok, type, g_current_scope);
  var = find_var_recursively(tok, g_current_scope);
  var->type = type;
  return gen_node_from_var(var);
}

Node* gen_node_from_var(Var* var){
  Node* node = new_node(ND_VAR_DEC, NULL, NULL);
  node->name = var->name;
  node->len = var->len;
  node->offset = var->offset;
  node->type = var->type;
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

// ENBF primary = ( expr ) | num | ident
Node* primary(){
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

// ENBF postfix = primary
//              | postfix "[" expr "]"
Node* postfix(){
  Node* node = primary();
  if(consume("[")){
	Node* expression = expr();
	if(expression == NULL){
	  error_at(token->str,"No expression after [\n");
	}
	expect("]");
	// postfix[expression] is parsed as *(postfix+expression)
	node = new_node(ND_ADD, node, expression);
	node = new_node(ND_DEREF, node, NULL);
	return node;
  }
  return node; // return primary
}

// ENBF ident = var | func
Node* ident(){
  Token* tok = consume_ident();
  Node* node;
  if(is_symbol("(")){
    node = func(tok);
  }else{ // variable
    node = var(tok);
  }
  return node;
}

// ENBF var
Node* var(Token* tok){
  char* node_name = calloc(1, tok->len+1);
  strncpy(node_name, tok->str, tok->len);
  node_name[tok->len] = '\0';
  Var* var = find_var_recursively(tok, g_current_scope);
  if(var == NULL){
    error_at(token->str, "Variable %s is not declared\n", node_name);
    exit(1);
  }

  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_VAR;
  node->name = node_name;
  node->len = tok->len;
  node->offset = var->offset;
  node->type = var->type;;
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

// ENBF unary = "+"? postfix
//            | "-"  postfix
//            | "&"  unary
//            | "*"  unary
//            | "sizeof" unary
Node* unary(){
  if(consume("+")){
    return postfix();
  }
  if(consume("-")){
    return new_node(ND_SUB,new_node_num(0), postfix());
  }
  if(consume("&")){
    return new_node(ND_ADDR, unary(), NULL);
  }
  if(consume("*")){
    return new_node(ND_DEREF, unary(), NULL);
  }
  if(consumeByKind(TK_SIZEOF)){
	Node* nodesize = unary();
	if(nodesize->type != NULL){
	  return new_node_num(get_type_size_byte(nodesize->type));
	}else{
	  error_at(token->str, "Invalid token for sizeof operator\n");
	  return NULL;
	}
  }
  return postfix();
}

// ENBF terminal symbol for number
Node* num(){
  return new_node_num(expect_number());
}

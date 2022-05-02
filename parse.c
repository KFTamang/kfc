#include "parse.h"


// global vector to contain string literals
StrLtr* g_string_literal;
// global variable for string literal counter
int g_num;

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

Switch_list* new_switch_node_list(int is_defalut, int case_num, node_list* nl){
  Switch_list* sl = calloc(1, sizeof(Switch_list));
  sl->next = NULL;
  sl->is_default = is_defalut;
  sl->case_num = case_num;
  sl->nl = nl;
  return sl;
}

void append_switch_node_list(int is_default, int case_num, Switch_list* sw_l, node_list* nl){
  if(sw_l == NULL){
    error_at(token->str, "Switch-case node list is blank\n");
  }
  Switch_list* new_sw = calloc(1, sizeof(Switch_list));
  if(is_default){
    new_sw->is_default = 1;
  }else{
    new_sw->case_num = case_num;
  }
  new_sw->nl = nl;

  Switch_list* sw = sw_l;
  while(sw->next){
    sw = sw->next;
  }
  sw->next = new_sw;  
}

StrLtr* get_and_append_strltr(char* string){
  StrLtr* new = calloc(1, sizeof(StrLtr));
  new->next = g_string_literal;
  new->string = string;
  char* label = calloc(1, strlen(".LCxxx")); // up to 999 string literals
  sprintf(label, ".LC%d", g_num);
  new->label = label;
  ++g_num;
  g_string_literal = new;
  return new;
}

int consume_constant(){
  if(is_kind(TK_NUM)){
    return expect_number();
  }
  if(is_kind(TK_IDENT)){
    Token* tok = consume_ident();
    Var* var = find_var_recursively(tok, g_current_scope);
    if(var == NULL){
      error_at(token->str, "Expected constant value\n");
    } 
    if(var->kind == VK_ENUM){
      return var->offset;
    }
    error_at(token->str, "Only enum element is allowed\n");
  }
}

void print_all_strltrs(){
  for(StrLtr* strltr=g_string_literal; strltr; strltr=strltr->next){
    printf("%s:\n", strltr->label);
    printf("  .string \"%s\"\n", strltr->string);
  }
}

void initialize_global_scope(){
  g_global_scope = gen_new_scope(NULL, GLOBAL);
}

void initialize_current_scope(){
  g_current_scope = g_global_scope;
}

Scope* get_global_scope(){
  return g_global_scope;
}

void enter_new_scope(){
  g_current_scope = gen_new_scope(g_current_scope, LOCAL);
}

void exit_current_scope(){
  g_current_scope = g_current_scope->parent;
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
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
      if(lhs->type == NULL){
//        error_at(token->str, "Type for leht hand side is not defined for nodes\n");
        break;
      }else if(rhs->type == NULL ){
//        error_at(token->str, "Type for right hand side is not defined for nodes\n");
        break;
      }
      if((lhs->type->ty == PTR || lhs->type->ty == ARRAY) && (rhs->type->ty == PTR || rhs->type->ty == ARRAY) ){
        error_at(token->str, "Operation between two pointers/arrays is not allowed\n");
        break;
      }
      if(lhs->type->ty == PTR || lhs->type->ty == ARRAY){
        node->type = lhs->type;
        break;
      } 
      if(rhs->type->ty == PTR || rhs->type->ty == ARRAY){
        node->type = rhs->type;
        break;
      } 
      node->type = new_type(INT, NULL);
      break;
    case ND_EQU:
    case ND_NEQ:
    case ND_LWT:
    case ND_LEQ:
    case ND_ASSIGN:
      break;
    case ND_ADDR:
      node->type = new_type(PTR, NULL);
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
  node->type = new_type(INT, NULL);
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
  new_var->type = type;
  new_var->size_byte = get_type_size_byte(type);
  new_var->offset = get_var_size_byte(g_current_scope) + get_type_size_byte(type);
  if(scope->sk == GLOBAL){
    new_var->is_global = 1;
  }else{
    new_var->is_global = 0;
  }
  new_var->kind = VK_VAR;
  scope->var = new_var;
}

void append_type_to_scope(Token* tok, Type* type, Scope* scope){
  Var* new_var = calloc(1, sizeof(Var));
  new_var->next = scope->var;
  char* var_name = calloc(1, tok->len+1);
  strncpy(var_name, tok->str, tok->len);
  var_name[tok->len] = '\0';
  new_var->name = var_name;
  new_var->len = tok->len;
  new_var->type = type;
  new_var->size_byte = get_type_size_byte(type);
  new_var->offset = get_var_size_byte(g_current_scope) + get_type_size_byte(type);
  if(scope->sk == GLOBAL){
    new_var->is_global = 1;
  }else{
    new_var->is_global = 0;
  }
  new_var->kind = VK_TYPE;
  scope->var = new_var;
}

Tag* find_tag_in_scope(Token* tok, Scope* scope){
  if(scope==NULL){
  	return NULL;
  }
  for(Tag* tag=scope->tag; tag; tag=tag->next){
    if(tag->len == tok->len && !memcmp(tok->str, tag->name, tag->len)){
      return tag;
    }
  }
  return NULL;
}

Tag* find_tag_in_function_scope(Token* tok, Scope* scope){
  if(scope==NULL){
  	return NULL;
  }
  Tag* tag = find_tag_in_scope(tok, scope);
  if(tag!=NULL){
	  return tag;
  }
  if(scope->sk==GLOBAL || scope->parent==NULL){
  	return NULL;
  }
  return find_tag_recursively(tok, scope->parent);
}

Tag* find_tag_recursively(Token* tok, Scope* scope){
  if(scope==NULL){
  	return NULL;
  }
  Tag* tag = find_tag_in_scope(tok, scope);
  if(tag!=NULL){
  	return tag;
  }
  if(scope->parent==NULL){
  	return NULL;
  }
  return find_tag_recursively(tok, scope->parent);
}

void append_tag_to_scope(Token* tok, Type* type, Scope* scope){
  Tag* new_tag = calloc(1, sizeof(Tag));
  new_tag->next = scope->tag;
  char* tag_name = calloc(1, tok->len+1);
  strncpy(tag_name, tok->str, tok->len);
  tag_name[tok->len] = '\0';
  new_tag->type = type;
  new_tag->name = tag_name;
  new_tag->len = tok->len;
  new_tag->is_complete = 1;
  if(scope->sk == GLOBAL){
    new_tag->is_global = 1;
  }else{
    new_tag->is_global = 0;
  }
  scope->tag = new_tag;
}

void append_incomplete_tag_to_scope(Token* tok, Scope* scope){
  Tag* new_tag = calloc(1, sizeof(Tag));
  new_tag->next = scope->tag;
  char* tag_name = calloc(1, tok->len+1);
  strncpy(tag_name, tok->str, tok->len);
  tag_name[tok->len] = '\0';
  new_tag->type = new_type(STRUCT, NULL);
  new_tag->name = tag_name;
  new_tag->len = tok->len;
  new_tag->is_complete = 0;
  if(scope->sk == GLOBAL){
    new_tag->is_global = 1;
  }else{
    new_tag->is_global = 0;
  }
  scope->tag = new_tag;
}

int get_type_size_byte(Type* type){
  if(type == NULL){
	error("Type is not specified\n");
	return 0;
  }
  int size = 0;
  switch(type->ty){
  case INT:	return 8;
  case PTR: return 8;
  case CHAR: return 1;
  case VOID: return 0;
  case ARRAY:	return type->array_size * get_type_size_byte(type->ptr_to);
  case STRUCT:
    for(Memlist* mem=type->mem; mem; mem=mem->next){
      size += get_type_size_byte(mem->type);
    }
    return size;
  case ENUM: return 8;
  default:
	error_at(token->str, "Type is not supported\n");
	return 0;
  }
}

int get_var_size_byte(Scope* scope){
  if(scope==NULL){
	return 0;
  }
  int var_size = 0;
  for(Var* var=scope->var; var; var=var->next){
	var_size += var->size_byte;
  }
  if(scope->sk == BLOCK){
	var_size += get_var_size_byte(scope->parent);
  }	
  return var_size;
}

// ENBF program = global_dec*
void program(){
  int i = 0;
  while(!at_eof()){
    code[i] = global_dec();
    ++i;
  }
  code[i] = NULL;
}

// ENBF global_dec = type_dec ident "(" lvar_dec? ")" "{" stmt* "}"
//                 | type_dec ident ";"
//                 | type_dec ";"
//                 | type_def ";"
Node* global_dec(){
  if(consumeByKind(TK_TYPEDEF)){
    type_def();
    expect(";");
    return new_node(ND_EMPTY, NULL, NULL);
  }
  Type* this_type = type_dec();
  if(this_type == NULL){
    error_at(token->str, "Unsupported syntax\n");
  }
  Token* tok = consume_ident();
  Node* node;
  // if global variable declaration
  if(consume(";")){
  	g_current_scope = g_global_scope;
	  node = new_var_node(this_type, tok);
	  return node;
  }else if(consume("[")){
    int array_size = expect_number();
    this_type = new_array_type(this_type, array_size);
  	g_current_scope = g_global_scope;
	  node = new_var_node(this_type, tok);
    expect("]");
    expect(";");
	  return node;
  }
  // function
  node = new_node(ND_FUNC_DEF, NULL, NULL);
  char* node_name = calloc(1, tok->len+1);
  strncpy(node_name, tok->str, tok->len);
  node->name = node_name;
  node->len = tok->len;
  node->type = this_type;
  node->is_global = 1;
  expect("(");
  // arguments
  enter_new_scope();
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
  // if prototype declaration
  if(consume(";")){
    exit_current_scope();
    return new_node(ND_EMPTY, NULL, NULL); // ignore for now
  }
  //function definition
  expect("{");
  // statements
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
  exit_current_scope();
  return node;
}

// ENBF stmt = expr ";" | "return" expr ";"
//           | "{" stmt* "}"
//           | "if" "(" expr ")" stmt ("else" stmt)?
//           | "while" "(" expr ")" stmt
//           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//           | var_dec ";"
//           | type_def ";"
//           | "switch" "(" expr "){" ( "case" num ":" stmt )* "}"
//           | "break" ";"
//           | "continue" ";"
Node* stmt(){
  Node* node;
  // if return statement
  if(consumeByKind(TK_RETURN)){
    node = new_node(ND_RETURN, expr(), NULL);
    expect(";");
  }else if(consume("{")){ // compound statement (block) {}
    enter_new_scope();
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
    exit_current_scope();
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
    enter_new_scope();
    expect("(");
    Node* init;
    Node* cond;
    Node* end;
    //    if(is_next_symbol(";")){ // empty initialize section
    if(consume(";")){
      init = NULL;
    }else{
      // initialize section
      // This section contains either nothing or expression or variable declaration
      Node* var_dec_node = var_dec();
      if(var_dec_node == NULL){
        init = expr();
      }else{
        init = var_dec_node;
      }
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
    exit_current_scope();
  }else if(consumeByKind(TK_TYPEDEF)){
      type_def();
      expect(";");
      return new_node(ND_EMPTY, NULL, NULL);
  }else if(consumeByKind(TK_SWITCH)){
    expect("(");
    node = expr();
    node = new_node(ND_SWITCH, node, NULL);
    expect(")");
    expect("{");
    if(!consumeByKind(TK_CASE)){
      error_at(token->str, "Switch sentence should contain at least one case\n");
    }
    int case_num = consume_constant();
    expect(":");
    node_list* nl = new_node_list(stmt());
    node->sw_l = new_switch_node_list(0, case_num, nl);
    while(!is_kind(TK_CASE) && !is_symbol("}")){
      nl = append_node_list(nl, stmt());
    }
    while(is_kind(TK_CASE) || is_kind(TK_DEFAULT)){
      if(consumeByKind(TK_CASE)){
        int case_num = consume_constant();
        expect(":");
        nl = new_node_list(stmt());
        append_switch_node_list(0, case_num, node->sw_l, nl);
        while(!is_kind(TK_CASE) && !is_symbol("}") && !is_kind(TK_DEFAULT)){
          nl = append_node_list(nl, stmt());
        }      
      }else if(consumeByKind(TK_DEFAULT)){
        expect(":");
        nl = new_node_list(stmt());
        append_switch_node_list(1, 0, node->sw_l, nl);
        while(!is_kind(TK_CASE) && !is_symbol("}")){
          nl = append_node_list(nl, stmt());
        }   
      }
    }
    expect("}");
    return node;
  }else if(consumeByKind(TK_BREAK)){
    expect(";");
    return new_node(ND_BREAK, NULL, NULL);
  }else if(consumeByKind(TK_CONTINUE)){
    expect(";");
    return new_node(ND_CONTINUE, NULL, NULL);
  }else{
    node = var_dec();
    if(node != NULL){
      expect(";");
      return node;
    }
    node = expr();
    if(node != NULL){
      expect(";");
    }else{
      return new_node(ND_EMPTY, NULL, NULL);
    }
  }
  return node;
}

// ENBF type_dec = ("int" | "char" | struct_dec | enum_dec) "*"*
Type* type_dec(){
  Type* type;
  int is_extern = consumeByKind(TK_EXTERN);
  if(consumeByKind(TK_TYPE_INT)){
    type = new_type(INT, NULL);
  }else if(consumeByKind(TK_TYPE_CHAR)){
    type = new_type(CHAR, NULL);
  }else if(consumeByKind(TK_TYPE_VOID)){
    type = new_type(VOID, NULL);
  }else if(consumeByKind(TK_STRUCT)){
    type = struct_dec();
  }else if(consumeByKind(TK_ENUM)){
    type = enum_dec();
  }else{
    Token* sn = save_snapshot();
    Token* tok = consume_ident();
    if(tok != NULL){
      Var* v_type = find_var_recursively(tok, g_current_scope);
      if(v_type == NULL || v_type->kind != VK_TYPE){
        revert_snapshot(sn);
        return NULL;
      }
      type = v_type->type;
    }else{
      revert_snapshot(sn);
      return NULL;
    }
  }
  while(consume("*")){
    type = new_type(PTR, type);
  }
  type->is_extern = is_extern;
  return type;
}

Type* struct_dec(){
  Token* tag_name = consume_ident();
  if(tag_name != NULL){
    // struct with tag name
    Tag* tag = find_tag_recursively(tag_name, g_current_scope);
    if(tag != NULL){
      // if struct tag is already declared
      if(tag->is_complete){
        // if complete
        if(consume("{")){
          error_at(token->str, "Struct %s is already declared\n", tag_name->str);
        }
        return tag->type;
      }else{
        // if incomplete
        if(consume("{")){
          tag->type->mem = get_member_list();
          tag->type->array_size = get_member_size(tag->type->mem);
          tag->is_complete = 1;
        }else{
          if(!is_symbol("*")){
            error_at(token->str, "Struct declaration is incomplete\n");
          }
        }
      }
      return tag->type;
    }else{
      // if tag name is not previously declared
      append_incomplete_tag_to_scope(tag_name, g_current_scope);
      tag = find_tag_in_function_scope(tag_name, g_current_scope);
      if(consume("{")){
        tag->type->mem = get_member_list();
        tag->type->array_size = get_member_size(tag->type->mem);
        tag->is_complete = 1;
        return tag->type;
      }else{
        return tag->type;
      }
    }
  }else{
    // struct without tag name
    expect("{");
  }
  Type* this_type = new_type(STRUCT, NULL);
  this_type->mem = get_member_list();
  this_type->array_size = get_member_size(this_type->mem);
  if(tag_name != NULL){
    append_tag_to_scope(tag_name, this_type, g_current_scope);
  }
  return this_type;
}

Memlist* get_member_list(){
  Type* type = type_dec();
  Token* tok = consume_ident();
  if(consume("[")){ // array member
    int array_size = expect_number();
    type = new_array_type(type, array_size);
    expect("]");
  }
  Memlist* head = new_memlist(type, tok);
  expect(";");
  while(!consume("}")){
    type = type_dec();
    tok = consume_ident();
    if(tok == NULL){ // if declaration is incomplete
      expect(";");
      continue; // skip appending to the member list
    }
    if(consume("[")){ // array member
      int array_size = expect_number();
      type = new_array_type(type, array_size);
      expect("]");
    }
    append_memlist(head, new_memlist(type, tok));
    expect(";");
  }
  // assign offsets to each of struct member
  int offset = 0;
  for(Memlist* mem = head; mem; mem=mem->next){
    mem->offset = offset;
    offset += get_type_size_byte(mem->type);
  }
  return head;
}

int get_member_size(Memlist* head){
  int offset = 0;
  for(Memlist* mem = head; mem; mem=mem->next){
    mem->offset = offset;
    offset += get_type_size_byte(mem->type);
  }
  return offset;
}

void append_memlist(Memlist* cur, Memlist* new){
  if(cur == NULL){
    error("No struct member\n");
  }
  if(strcmp(cur->name, new->name) == 0){
    error_at(token->str, "Struct member '%s' already exists\n", cur->name);
  }
  // go to the end of member list
  while(cur->next != NULL){
    if(strcmp(cur->name, new->name) == 0){
      error_at(token->str, "Struct member '%s' already exists\n", cur->name);
    }
    cur = cur->next;
  }
  cur->next = new;
}

Memlist* new_memlist(Type* type, Token* tok){
  Memlist* new = calloc(1,sizeof(Memlist));
  // extract name from token string
  new->name = calloc(1, tok->len+1);
  strncpy(new->name, tok->str, tok->len);
  new->name[tok->len] = '\0';
  new->type = type;
  return new;
}

Type* enum_dec(){
  expect("{");
  int i = 0;
  while(!consume("}")){
    Token* tok = consume_ident();
    append_enum(tok, i);
    ++i;
    consume(",");
  }
  return new_type(ENUM, NULL);
}

void append_enum(Token* tok, int num){
  Var* var = find_var_in_scope(tok, g_current_scope);
  if(var){
    error_at(token->str, "Variable '%s' is already declared\n", get_name_from_token(tok));
  }
  append_enum_to_scope(tok, num, g_current_scope);
}

void append_enum_to_scope(Token* tok, int num, Scope* scope){
  Var* new_var = calloc(1, sizeof(Var));
  new_var->next = scope->var;
  char* var_name = calloc(1, tok->len+1);
  strncpy(var_name, tok->str, tok->len);
  var_name[tok->len] = '\0';
  new_var->name = var_name;
  new_var->len = tok->len;
  new_var->type = new_type(ENUM, NULL);
  new_var->kind = VK_ENUM;
  new_var->size_byte = 8;
  new_var->offset = num;
  if(scope->sk == GLOBAL){
    new_var->is_global = 1;
  }else{
    new_var->is_global = 0;
  }
  scope->var = new_var;
}

Type* new_array_type(Type* base, int size){
  Type* new_array = calloc(1, sizeof(Type));
  new_array->ty = ARRAY;
  new_array->ptr_to = base;
  new_array->array_size = size;
  return new_array;
}

void type_def(){
  Type* type = type_dec();
  Token* tok = consume_ident();
  if(tok == NULL){
    error_at(token->str, "ERROR: no new type name specified\n");
  }
  Var* var = find_var_recursively(tok, g_current_scope);
  if(var != NULL){
    error_at(token->str, "ERROR: Typedef '%s' is already used\n", get_name_from_token(tok));
  }
  append_type_to_scope(tok, type, g_current_scope);
}

// ENBF var_dec = type_dec var ("[" num "]")?
//              | type_dec var ("[" num "]")? = expr
//              | type_dec
Node* var_dec(){
  Type* this_type = type_dec();
  if(this_type == NULL){
    return NULL;
  }
  Token* tok = consume_ident();
  Node* node = new_var_node(this_type, tok);
  if(!consume("=")){
    return node;
  }else{ // declaration with initialization
    // initialization is treated as : 
    // int x = expr(); -> int x; x = expr();
    node = var(tok);
    node = new_node(ND_ASSIGN, node, expr());
    return node;
  }
}

Node* new_var_node(Type* type, Token* tok){
  if(tok == NULL){
    return new_node(ND_EMPTY, NULL, NULL);
  }
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
  node->is_global = var->is_global;
  return node;
}

// ENBF expr = assign
Node* expr(){
  return assign();
}

// ENBF assign = logical ( ( "=" | "+=" ) assign)?
Node* assign(){
  Node* node = logical();
  if (consume("=")){
    node = new_node(ND_ASSIGN, node, assign());
  }else if(consume("+=")){
    Node* logi = node;
    node = new_node(ND_ADD, logi, assign());
    node = new_node(ND_ASSIGN, logi, node);
  }
  return node;
}

// ENBF logical = equality ( "&&" equality ) *
Node* logical(){
  Node* node = equality();

  for(;;){
    if(consume("&&")){
      node = new_node(ND_AND, node, equality());
    }else if(consume("||")){
      node = new_node(ND_OR, node, equality());
    }else{
      return node;
    }
  }
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
    }else if(consume("\%")){
      node = new_node(ND_MOD, node, unary());
    }else{
      return node;
    }
  }
}

// ENBF primary = ( expr ) | num | ident | string-literal
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
  if(is_kind(TK_STRING)){
    Node* node = string_literal();
    return node;
  }
  return num();
}

// ENBF postfix = prefix
//              | postfix "[" expr "]"
//              | postfix "." ident
//              | postfix "->" ident
Node* postfix(){
  Node* node = prefix();
  while(1){
    if(consume("[")){
      Node* expression = expr();
      if(expression == NULL){
        error_at(token->str,"No expression after [\n");
      }
      expect("]");
      // postfix[expression] is parsed as *(postfix+expression)
      Node*	add_node = new_node(ND_ADD, node, expression);
      node = new_node(ND_DEREF, add_node, NULL);
      node->type = add_node->type->ptr_to;
    }else if(consume(".")){
      node = struct_mem(node);
    }else if(consume("->")){
      // postfix->ident is parsed as (*postfix).ident
      node = deref(node);
      node = struct_mem(node);
    }else{
      return node;
    }
  }
}

Node* struct_mem(Node* node){
  Token* tok = consume_ident();
  if(tok == NULL){
    error_at(token->str, "No member name after . operator\n");
  }
  Type* struct_type = node->type;
  Memlist* memlist = find_member(struct_type, get_name_from_token(tok));
  Node* member = new_node(ND_STRUCT_MEM, node, NULL);
  member->member = memlist;
  member->type = memlist->type;
  return member;
}

Memlist* find_member(Type* struct_type, char* name){
  if(struct_type->ty != STRUCT){
    error_at(token->str, "Variable is not struct\n");
  }
  for(Memlist* mem=struct_type->mem; mem; mem=mem->next){
    if(strcmp(mem->name, name) == 0){
      return mem;
    }
  }
  error_at(token->str, "No such member as '%s' in struct\n", name);
}

// ENBF prefix = primary
//             | "++" primary
Node* prefix(){
  Node* node;
  if(consume("++")){
    node = primary();
    // ++prefix is parsed as prefix = prefix + 1
    Node*	add_node = new_node(ND_ADD, node, new_node_num(1));
    node = new_node(ND_ASSIGN, node, add_node);
  }else{
    node = primary();
  }
  return node;
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

  // if identifier is enum element, return integer associated with it
  if(var->kind == VK_ENUM){
    return new_node_num(var->offset);
  }

  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_VAR;
  node->name = node_name;
  node->len = tok->len;
  node->offset = var->offset;
  node->type = var->type;
  node->is_global = var->is_global;
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
//            | "!"  postfix
//            | "&"  unary
//            | "*"  unary
//            | "sizeof" (unary | type_dec | "(" type_dec ")" )
Node* unary(){
  if(consume("+")){
    return postfix();
  }
  if(consume("-")){
    return new_node(ND_SUB,new_node_num(0), postfix());
  }
  if(consume("!")){
    return new_node(ND_NOT, postfix(), NULL);
  }
  if(consume("&")){
    return ref(unary());
  }
  if(consume("*")){
    return deref(unary());
  }
  if(consumeByKind(TK_SIZEOF)){
    Token* sp = save_snapshot();
    int use_parenth = 0;
    if(consume("(")){
      use_parenth = 1;
    }
    Type* nodetype = type_dec();
    Node* nodesize;
    if(nodetype == NULL){
      revert_snapshot(sp);
      nodesize = unary();
      nodetype = nodesize->type;
    }else{
      if(use_parenth){
        expect(")");
      }
    }
    if(nodetype != NULL){
      return new_node_num(get_type_size_byte(nodetype));
    }else{
      error_at(token->str, "Invalid token for sizeof operator\n");
      return NULL;
    }
  }
  return postfix();
}

Node* ref(Node* una){
    Node* node = new_node(ND_ADDR, una, NULL);
    node->type = new_type(PTR, una->type);
    return node;
}

Node* deref(Node* una){
    Node* node = new_node(ND_DEREF, una, NULL);
    node->type = una->type->ptr_to;
    return node;
}

// ENBF terminal node string-literal
// string literal is actually a global variable with char* type
Node* string_literal(){
  Token* tok = consume_string_literal();
  Node* node = new_node(ND_VAR, NULL, NULL);
  char* string = calloc(1, tok->len+1);
  strncpy(string, tok->str, tok->len);
  string[tok->len] = '\0';
  StrLtr* strltr = get_and_append_strltr(string);
  node->name = strltr->label;
  node->len = tok->len;
  node->is_global = 1;
  node = new_node(ND_ADDR, node, NULL);
  node->type = new_type(PTR, new_type(CHAR, NULL));
  return node;
}

// ENBF terminal symbol for number
Node* num(){
  if(is_kind(TK_NUM)){
    return new_node_num(expect_number());
  }else{
    return NULL;
  }
}

Token* save_snapshot(){
  return token;
}

void revert_snapshot(Token* sp){
  token = sp;
}

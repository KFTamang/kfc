#include "kfc.h"

// node lists
node_list* new_node_list(Node* node);
node_list* append_node_list(node_list* current, Node* data);

// switch-case sentence
Switch_list* new_switch_node_list(int case_num, node_list* nl);
void append_switch_node_list(int case_num, Switch_list* sw_l, node_list* nl);

// string literal
StrLtr* get_and_append_strltr(char* string);

// scope
void enter_new_scope();
void exit_current_scope();

// identifiers
Var* find_var(Token* tok);
Var* find_var_recursively(Token* tok, Scope* scope);
Var* find_var_in_function_scope(Token* tok, Scope* scope);
Var* find_var_in_scope(Token* tok, Scope* scope);

// tag names
Tag* find_tag_recursively(Token* tok, Scope* scope);
Tag* find_tag_in_function_scope(Token* tok, Scope* scope);
Tag* find_tag_in_scope(Token* tok, Scope* scope);
Memlist* get_member_list();
int get_member_size(Memlist* head);
void append_incomplete_tag_to_scope(Token* tok, Scope* scope);

// member list for struct
void append_memlist(Memlist* cur, Memlist* new);
Memlist* new_memlist(Type* type, Token* tok);
Memlist* find_member(Type* struct_type, char* name);

// enumeration type
void append_enum(Token* tok, int num);
void append_enum_to_scope(Token* tok, int num, Scope* scope);

// global type struct for integer number


Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node_num(int val);
void append_var_to_scope(Token* tok, Type* type, Scope* scope);
size_t get_var_size_byte(Scope* scope);
Type* type_dec();
void type_def();
Type* struct_dec();
Type* enum_dec();
Type* new_array_type(Type* base, size_t size);
Node* new_var_node(Type* type, Token* tok);
Node* gen_node_from_var(Var* var);


Node* global_dec();
Node* stmt();
Node* var_dec();
Node* expr();
Node* assign();
Node* logical();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* primary();
Node* postfix();
Node* struct_mem(Node* node);
Node* ident();
Node* var(Token* tok);
Node* func(Token* tok);
Node* unary();
Node* ref(Node* una);
Node* deref(Node* una);
Node* string_literal();
Node* num();

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static const int VAR_NAME_SIZE = 512;
static const int MAX_ARG_NUM = 6;
static const char* ARG_REG[] = {"rdi","rsi","rdx","rcx","r8","r9"};
//static const int MAX_SYMBOL_NUM = 65535; // 16bit for now
#define MAX_SYMBOL_NUM  (65535) // 16bit for now

// tokenizer
typedef enum{
  TK_RESERVED,  // symbol
  TK_IDENT,     // identifier
  TK_NUM,       // number
  TK_RETURN,    // return
  TK_IF,        // if
  TK_ELSE,      // else
  TK_WHILE,     // while loop
  TK_FOR,       // for loop
  TK_EOF,       // end of file
  TK_TYPE_INT,  // type int
  TK_TYPE_CHAR, // type char
  TK_TYPE_VOID, // type void
  TK_SIZEOF,    // sizeof operator
  TK_STRING,    // string literal
  TK_STRUCT,    // structure declaration
  TK_ENUM,      // enumaration
  TK_TYPEDEF,   // typedef
  TK_SWITCH,    // switch
  TK_CASE,      // case
} TokenKind;

typedef struct Token Token;

struct Token{
  TokenKind kind; // token type symbol, number, end of file
  Token *next;    // pointer to next token
  int val;        // integer value if number
  char *str;      // token content if symbol
  int len;        // length of token
};

bool consume(char* op);
bool consumeByKind(TokenKind tkind);
int is_alnum(char c);
Token* get_and_consume_token(TokenKind tk);
Token* consume_ident();
Token* consume_string_literal();
void expect(char* op);
void expect_in_future(char* op);
bool is_symbol(char* op);
bool is_kind(TokenKind tkind);
bool is_next_symbol(char* op);
bool is_next_kind(TokenKind tkind);
int expect_number();
bool at_eof();
Token* new_token(TokenKind kind, Token* cur, char* str, int len);
Token* tokenize(char* p);

Token* save_snapshot();
void revert_snapshot(Token* sp);

// curent token in interest
Token* token;
char* user_input;

// abstract syntax tree kind
typedef enum{
  ND_ADD, // + 0
  ND_SUB, // - 1
  ND_MUL, // * 2
  ND_DIV, // / 3
  ND_EQU, // == 4
  ND_NEQ, // != 5
  ND_LWT, // < 6
  ND_LEQ, // <= 7
  ND_NUM, // number 8
  ND_ASSIGN, // assignment 9
  ND_VAR, // local variable 10
  ND_RETURN, // return statement 11
  ND_IF, // if statement 12
  ND_WHILE, // while statement 13
  ND_FOR,      // for loop 14
  ND_BLOCK, // compound statement 15
  ND_FUNC, // function call
  ND_FUNC_DEF, // function definition
  ND_ADDR, // pointer address
  ND_DEREF, // pointer dereference
  ND_VAR_DEC, // local variable declaration
  ND_TYPE,  // type
  ND_STRUCT_MEM, // struct member
  ND_EMPTY, // empty node
  ND_AND,  // logical expression AND
  ND_OR,  // logical expression OR
  ND_NOT, // unary operator NOT
  ND_SWITCH, // switch case
} NodeKind;

typedef struct Node Node;
typedef struct node_list node_list;
typedef struct funcs funcs;
typedef struct Type Type;
typedef struct Memlist Memlist;
typedef struct Var Var;
typedef struct StrLtr StrLtr;
typedef struct Switch_list Switch_list;

// abstract syntax tree struct
struct Node {
  NodeKind kind; // node kind
  Node* lhs;     // pointer to left hand side
  Node* rhs;     // pointer to right hand side
  int val;       // value if kind is ND_NUM
  int offset;    // offset if kind is ND_VAR
  Node* cond;    // condition for "if"/"for" statement
  Node* then;    // then-statement for "if" statement
  Node* els;     // else-statement for "if" statement
  Node* init;    // initialize section for "for" loop
  Node* end;     // end section for "for" loop
  node_list* comp_stmt; // compound statement
  char* name;    // name of function
  int len;       // length of name of function
  node_list* func_args; // arguments of function
  int var_size_byte; // total size of local variables in bytes
  Type* type; // type of the variable
  int is_global; // flag for global variable
  Memlist* member; // struct member 
  Switch_list* sw_l; // switch-case sentence
};

struct node_list{
  Node* data;
  node_list* next;
};

node_list* new_node_list(Node* node);
node_list* append_node_list(node_list* current, Node* data);

struct Switch_list{
  node_list* nl;
  int case_num;
  Switch_list* next;
};
Switch_list* new_switch_node_list(int case_num, node_list* nl);
void append_switch_node_list(int case_num, Switch_list* sw_l, node_list* nl);

// vactor for string literal appended in parser for later code generation
struct StrLtr{
  char* string;
  char* label;
  StrLtr* next; 
};
StrLtr* get_and_append_strltr(char* string);
void print_all_strltrs();

typedef struct Scope Scope;
typedef struct Tag Tag;
typedef enum {
  GLOBAL,
  LOCAL,
  BLOCK
}ScopeKind;
struct Scope{
  Scope* parent;
  Var* var;
  Tag* tag;
  ScopeKind sk;
};
// global char for current scope name
//extern Scope* g_global_scope;
//extern Scope* g_current_scope;
Scope* gen_new_scope(Scope* parent, ScopeKind sk);
void enter_new_scope();
void exit_current_scope();

typedef enum{
  VK_VAR,
  VK_ENUM,
  VK_TYPE,
}VarKind;

// local variable
struct Var{
  Var* next; // next variable or NULL
  char* name; // name of variable
  int len;    // length of name
  int offset; // offset from RBP
  int size_byte;    // size of the variable in byte
  Type* type; // type
  int is_global; // flag for global variables
  VarKind kind; // kind : variable, enum
};
Var* find_var(Token* tok);
// a chain of local variables
Var* find_var_recursively(Token* tok, Scope* scope);
Var* find_var_in_function_scope(Token* tok, Scope* scope);
Var* find_var_in_scope(Token* tok, Scope* scope);

struct Tag{
  Tag* next;
  char* name;
  int len;
  Type* type;
  int is_global;
  int is_complete;
};
Tag* find_tag_recursively(Token* tok, Scope* scope);
Tag* find_tag_in_function_scope(Token* tok, Scope* scope);
Tag* find_tag_in_scope(Token* tok, Scope* scope);
Memlist* get_member_list();
int get_member_size(Memlist* head);
void append_incomplete_tag_to_scope(Token* tok, Scope* scope);

typedef enum{
  INT,
  CHAR,
  VOID,
  PTR,
  ARRAY,
  STRUCT,
  ENUM,
}TY;

struct Type{
  TY ty;
  Type *ptr_to;
  size_t array_size;
  Memlist* mem;
};

struct Memlist{
  Type* type;
  char* name;
  int offset;
  Memlist* next;
};

void append_memlist(Memlist* cur, Memlist* new);
Memlist* new_memlist(Type* type, Token* tok);
Memlist* find_member(Type* struct_type, char* name);

void append_enum(Token* tok, int num);
void append_enum_to_scope(Token* tok, int num, Scope* scope);

// global type struct for integer number
static Type g_type_int = {INT, NULL};
static Type g_type_ptr = {PTR, NULL};

// local variable information for function definition
//struct lvar_info{
//  LVar* lvar;     // a chain of local variable
//  int total_byte; // total size of local vars in byte
//};


// functions
struct funcs{
  funcs* next; // next function or NULL
  char* name; // name of function
  int len;    // length of name
};
funcs* find_funcs(Token* tok);
// a chain of functions
funcs* functions;

// global array for program nodes
Node* code[10000];

// global number for label
int g_label_num;

Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node_num(int val);
Type* new_type(TY ty, Type* type);
void append_var_to_scope(Token* tok, Type* type, Scope* scope);
size_t get_type_size_byte(Type* type);
size_t get_var_size_byte(Scope* scope);
void program();
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


// code generator
void tree_print(Node* node, int i);
void gen(Node* node);
void gen_lval(Node* node);
void gen_node_list(node_list* nl);
void gen_func_args(node_list* fa);
void gen_global_var();
void switch_case(Node* node);

// error messager
void error(char* fmt, ...);
void error_at(char* loc, char* fmt, ...);
int round_up_to_8(int in);

// file reader
char* read_file(char* filename);
char* get_name_from_token(Token* tok);
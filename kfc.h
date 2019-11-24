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
  TK_RESERVED, // symbol
  TK_IDENT,    // identifier
  TK_NUM,      // number
  TK_RETURN,   // return
  TK_IF,       // if
  TK_ELSE,     // else
  TK_WHILE,    // while loop
  TK_FOR,      // for loop
  TK_EOF,      // end of file
  TK_TYPE_INT, // type int 
  TK_SIZEOF,   // sizeof operator
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
Token* consume_ident();
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
  ND_LVAR, // local variable 10
  ND_RETURN, // return statement 11
  ND_IF, // if statement 12
  ND_WHILE, // while statement 13
  ND_FOR,      // for loop 14
  ND_BLOCK, // compound statement 15
  ND_FUNC, // function call
  ND_FUNC_DEF, // function definition
  ND_ADDR, // pointer address
  ND_DEREF, // pointer dereference
  ND_LVAR_DEC, // local variable declaration
  ND_TYPE,  // type
} NodeKind;

typedef struct Node Node;
typedef struct node_list node_list;
typedef struct lvar_info lvar_info;
typedef struct funcs funcs;
typedef struct Type Type;

// abstract syntax tree struct
struct Node {
  NodeKind kind; // node kind
  Node* lhs;     // pointer to left hand side
  Node* rhs;     // pointer to right hand side
  int val;       // value if kind is ND_NUM
  int offset;    // offset if kind is ND_LVAR
  Node* cond;    // condition for "if"/"for" statement
  Node* then;    // then-statement for "if" statement
  Node* els;     // else-statement for "if" statement
  Node* init;    // initialize section for "for" loop
  Node* end;     // end section for "for" loop
  node_list* comp_stmt; // compound statement
  char* name;    // name of function
  int len;       // length of name of function
  node_list* func_args; // arguments of function
  lvar_info* lv_i; // infomation of local variables in function
  int lvar_size_byte; // total size of local variables in bytes
  Type* type; // type of the variable
};

struct node_list{
  Node* data;
  node_list* next;
};

node_list* new_node_list(Node* node);
node_list* append_node_list(node_list* current, Node* data);

typedef struct LVar LVar;

// local variable
struct LVar{
  LVar* next; // next variable or NULL
  char* name; // name of variable
  int len;    // length of name
  int offset; // offset from RBP
  int size_byte;    // size of the variable in byte
  char* scope_name; // name of the scope function
  Type* type; // type
};

typedef enum{
  INT,
  PTR,
  ARRAY,
}TY;

struct Type{
  TY ty;
  Type *ptr_to;
  size_t array_size;
};

// global type struct for integer number
static Type g_type_int = {INT, NULL};
static Type g_type_ptr = {PTR, NULL};

// local variable information for function definition
struct lvar_info{
  LVar* lvar;     // a chain of local variable
  int total_byte; // total size of local vars in byte
};

// a chain of local variables
LVar* locals;

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
Node* code[100];

// global number for label
int g_label_num;

// global char for current scope name
char* g_current_scope;

Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node_num(int val);
Type* new_type(TY ty, Type* type);
LVar* find_lvar(Token* tok);
void append_lvar(Token* tok, Type* type);
size_t get_type_size_byte(Type* type);
int get_lvar_size_byte();
void program();
Type* type_def();
Type* new_array_type(Type* base, size_t size);

Node* func_def();
Node* stmt();
Node* lvar_dec();
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* primary();
Node* postfix();
Node* ident();
Node* lvar(Token* tok);
Node* func(Token* tok);
Node* unary();
Node* num();


// code generator
void tree_print(Node* node, int i);
void gen(Node* node);
void gen_lval(Node* node);
void gen_node_list(node_list* nl);
void gen_func_args(node_list* fa);

// error messager
void error(char* fmt, ...);
void error_at(char* loc, char* fmt, ...);


#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VAR_NAME_SIZE (512)
#define MAX_ARG_NUM (6)
#define MAX_SYMBOL_NUM (65535) // 16bit for now
extern char* ARG_REG[6];
//static const int MAX_SYMBOL_NUM = 65535; // 16bit for now

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
  TK_BREAK,     // break
  TK_DEFAULT,   // default
  TK_CONTINUE,  // continue
  TK_EXTERN,    // extern
} TokenKind;

typedef struct Token Token;

struct Token{
  TokenKind kind; // token type symbol, number, end of file
  Token *next;    // pointer to next token
  int val;        // integer value if number
  char *str;      // token content if symbol
  int len;        // length of token
};

int consume(char* op);
int consumeByKind(TokenKind tkind);
int is_alnum(char c);
Token* get_and_consume_token(TokenKind tk);
Token* consume_ident();
Token* consume_string_literal();
void expect(char* op);
void expect_in_future(char* op);
int is_symbol(char* op);
int is_kind(TokenKind tkind);
int is_next_symbol(char* op);
int is_next_kind(TokenKind tkind);
int expect_number();
int at_eof();
Token* new_token(TokenKind kind, Token* cur, char* str, int len);
Token* tokenize(char* p);

Token* save_snapshot();
void revert_snapshot(Token* sp);

// curent token in interest
extern Token* token;
extern char* user_input;

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
  ND_BREAK,  // break
  ND_DEFAULT, // default in switch-case
  ND_CONTINUE, // continue
  ND_MOD, // modulo operation
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


struct Switch_list{
  node_list* nl;
  int case_num;
  int is_default;
  Switch_list* next;
};

// vactor for string literal appended in parser for later code generation
struct StrLtr{
  char* string;
  char* label;
  StrLtr* next; 
};

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

struct Tag{
  Tag* next;
  char* name;
  int len;
  Type* type;
  int is_global;
  int is_complete;
};

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
  int array_size;
  Memlist* mem;
  int is_extern;
};

struct Memlist{
  Type* type;
  char* name;
  int offset;
  Memlist* next;
};

// functions
struct funcs{
  funcs* next; // next function or NULL
  char* name; // name of function
  int len;    // length of name
};
funcs* find_funcs(Token* tok);
// a chain of functions
extern funcs* functions;

// global array for program nodes
extern Node* code[10000];

// global number for label
extern int g_label_num;

// parse function used in main()
void initialize_global_scope();
void initialize_current_scope();
Scope* get_global_scope();
void program();
void print_all_strltrs();
Scope* gen_new_scope(Scope* parent, ScopeKind sk);
int get_type_size_byte(Type* type);
Type* new_type(TY ty, Type* type);


// code generator
void gen(Node* node);
void gen_global_var();
// marker of loop type for break statement
typedef enum{
  LT_NONE,
  LT_SWITCH,
  LT_FOR,
  LT_WHILE
}LoopType;

typedef struct BreakMarker BreakMarker;

struct BreakMarker{
  LoopType type;
  int label_number;
  BreakMarker* prev_marker;
};

extern BreakMarker* bm;
void enterNewLoop(LoopType new_loop_type, int new_label_number);
void exitLoop();

// error messager
void error(char* fmt, ...);
void error_at(char* loc, char* fmt, ...);
int round_up_to_8(int in);

// file reader
char* read_file(char* filename);
char* get_name_from_token(Token* tok);

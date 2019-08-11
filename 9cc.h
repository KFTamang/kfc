#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenizer
typedef enum{
  TK_RESERVED, // symbol
  TK_IDENT,    // identifier
  TK_NUM,      // number
  TK_RETURN,   // return
  TK_IF,       // if
  TK_EOF,      // end of file
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
int expect_number();
bool at_eof();
Token* new_token(TokenKind kind, Token* cur, char* str, int len);
Token* tokenize(char* p);

// curent token in interest
Token* token;
char* user_input;

// abstract syntax tree kind
typedef enum{
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_EQU, // ==
  ND_NEQ, // !=
  ND_LWT, // <
  ND_LEQ, // <=
  ND_NUM, // number
  ND_ASSIGN, // assignment
  ND_LVAR, // local variable
  ND_RETURN, // return statement
  ND_IF, // if statement
} NodeKind;

typedef struct Node Node;

// abstract syntax tree struct
struct Node {
  NodeKind kind; // node kind
  Node* lhs;     // pointer to left hand side
  Node* rhs;     // pointer to right hand side
  int val;       // value if kind is ND_NUM
  int offset;    // offset if kind is ND_LVAR
};

typedef struct LVar LVar;

// local variable
struct LVar{
  LVar* next; // next variable or NULL
  char* name; // name of variable
  int len;    // length of name
  int offset; // offset from RBP
};

// a chain of local variables
LVar* locals;

// global array for program nodes
Node* code[100];

// global number for label
int label_num;

Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node_num(int val);
LVar* find_lvar(Token* tok);

void program();

Node* stmt();
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* term();
Node* unary();
Node* num();

// code generator
void tree_print(Node* node, int i);
void gen(Node* node);
void gen_lval(Node* node);

// error messager
void error(char* fmt, ...);
void error_at(char* loc, char* fmt, ...);
  


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
  TK_ELSE,     // else
  TK_WHILE,    // while loop
  TK_FOR,      // for loop
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
bool is_next_symbol(char* op);
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
} NodeKind;

typedef struct Node Node;

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
int g_label_num;

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
  


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
Token* consume_ident();
void expect(char* op);
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

// global array for program nodes
Node* code[100];

Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node_num(int val);

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
  


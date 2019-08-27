#include "kfc.h"

bool consume(char* op){
  if(token->kind != TK_RESERVED ||
     strlen(op) != token->len   ||
     memcmp(token->str, op, token->len) ){
    return false;
  }
  token = token->next;
  return true;
}

bool consumeByKind(TokenKind tkind){
  if(token->kind == tkind){
    token = token->next;
    return true;
  }
  return false;
}

int is_alnum(char c){
  return (('a' <= c && c <= 'z') ||
	  ('A' <= c && c <= 'Z') ||
	  ('0' <= c && c <= '9') ||
	  ( c == '_') );
}

Token* consume_ident(){
  Token* ret = NULL;
  if(token->kind == TK_IDENT ){
    ret = token;
    token = token->next;
  }
  return ret;
}

void expect(char* op){
  if(token->kind != TK_RESERVED ||
     strlen(op) != token->len   ||
     memcmp(token->str, op, token->len) ){
    error_at(token->str, "Not %s", op);
  }
  token = token->next;
}

void expect_in_future(char* op){
  if(token->next->kind != TK_RESERVED ||
     strlen(op) != token->next->len   ||
     memcmp(token->next->str, op, token->next->len) ){
    error_at(token->next->str, "Not %s", op);
  }
  token->next = token->next->next;
}

bool is_next_symbol(char* op){
  if(token->next->kind != TK_RESERVED ||
     strlen(op) != token->next->len   ||
     memcmp(token->next->str, op, token->next->len)){
    return false;
  }
  return true;
}


int expect_number(){
  if(token->kind != TK_NUM){
    error_at(token->str, "Not a number");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof(){
  return token->kind == TK_EOF;
}

Token* new_token(TokenKind kind, Token* cur, char* str, int len){
  Token* tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

Token* tokenize(char* p){
  Token head;
  head.next = NULL;
  Token* cur = &head;

  while(*p){
    if (isspace(*p)){
      p++;
      continue;
    }
    if(strncmp(p,"==",2) == 0 || strncmp(p,">=",2) == 0 ||
       strncmp(p,"<=",2) == 0 || strncmp(p,"!=",2) == 0 ){
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'||
	*p == '<' || *p == '>' || *p == ';' || *p == '=' || *p == '{' || *p == '}'){
      cur = new_token(TK_RESERVED, cur, p, 1);
      p++;
      continue;
    }
    if (isdigit(*p)){
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    if (is_alnum(*p)){
      int i = 1;
      while(is_alnum(*(p+i))){
	++i;
      }
      // if "if"
      if(i==2 && strncmp(p, "if", i)==0){ 
	cur = new_token(TK_IF, cur, p, i);
	p += i;
	continue;
      }
      // if else
      if(i==4 && strncmp(p, "else", i)==0){ 
	cur = new_token(TK_ELSE, cur, p, i);
	p += i;
	continue;
      }
      // if return
      if(i==6 && strncmp(p, "return", i)==0){ 
	cur = new_token(TK_RETURN, cur, p, i);
	p += i;
	continue;
      }
      // if while
      if(i==5 && strncmp(p, "while", i)==0){ 
	cur = new_token(TK_WHILE, cur, p, i);
	p += i;
	continue;
      }
      // if for
      if(i==3 && strncmp(p, "for", i)==0){ 
	cur = new_token(TK_FOR, cur, p, i);
	p += i;
	continue;
      }
      // local var
      cur = new_token(TK_IDENT, cur, p, i);
      p += i;
      continue;
    }

    error_at(cur->str, "Failed tokenizing\n");
  }

  new_token(TK_EOF, cur, p, 1);
  return head.next;

}


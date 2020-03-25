#include "kfc.h"

int consume(char* op){
  if(token->kind != TK_RESERVED ||
     strlen(op) != token->len   ||
     memcmp(token->str, op, token->len) ){
    return false;
  }
  token = token->next;
  return true;
}

int consumeByKind(TokenKind tkind){
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

Token* get_and_consume_token(TokenKind tk){
  Token* ret = NULL;
  if(token->kind == tk ){
    ret = token;
    token = token->next;
  }
  return ret;  
}

Token* consume_ident(){
  return get_and_consume_token(TK_IDENT);
}

Token* consume_string_literal(){
  return get_and_consume_token(TK_STRING);
}

void expect(char* op){
  if(token->kind != TK_RESERVED ||
     strlen(op) != token->len   ||
     memcmp(token->str, op, token->len) ){
    error_at(token->str, "Not %s\n", op);
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

int is_symbol(char* op){
  if(token->kind != TK_RESERVED ||
     strlen(op) != token->len   ||
     memcmp(token->str, op, token->len)){
    return false;
  }
  return true;
}

int is_kind(TokenKind tkind){
  if(token->kind != tkind){
    return false;
  }
  return true;
}

int is_next_symbol(char* op){
  if(token->next->kind != TK_RESERVED ||
     strlen(op) != token->next->len   ||
     memcmp(token->next->str, op, token->next->len)){
    return false;
  }
  return true;
}

int is_next_kind(TokenKind tkind){
  if(token->next->kind != tkind){
    return false;
  }
  return true;
}

int expect_number(){
  if(token->kind != TK_NUM){
    error_at(token->str, "Not a number\n");
  }
  int val = token->val;
  token = token->next;
  return val;
}

int at_eof(){
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

int tokenize_if_keyword_matches(char* p, Token** cur, int i, char* keyword, TokenKind tk){
  int len = strlen(keyword);
  if(i==len && strncmp(p, keyword, i)==0){ 
    *cur = new_token(tk, *cur, p, i);
    return i;
  }
  return 0;
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
    if(*p == '\n'){
      p++;
      continue;
    }
    if(strncmp(p,"//",2) == 0 ||
       strncmp(p,"#", 1) == 0){ // for now ignore preprocessor directives
      while(*p != '\n'){
        p++;
      }
      continue;
    }
    if(strncmp(p,"/*",2) == 0){
      while(strncmp(p,"*/",2) != 0){
        p++;
      }
      p+=2;
      continue;
    }
    if(strncmp(p,"==",2) == 0 || strncmp(p,">=",2) == 0 ||
       strncmp(p,"<=",2) == 0 || strncmp(p,"!=",2) == 0 ||
       strncmp(p,"->",2) == 0 || strncmp(p,"&&",2) == 0 ||
       strncmp(p,"||",2) == 0 || strncmp(p,"++",2) == 0){
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'||
      	*p == '<' || *p == '>' || *p == ';' || *p == '=' || *p == '{' || *p == '}'||
      	*p == ',' || *p == '&' || *p == '[' || *p == ']' || *p == '.' || *p == '!'||
        *p == ':'){
      cur = new_token(TK_RESERVED, cur, p, 1);
      p++;
      continue;
    }
    if(*p == '"'){
      p++;
      int i = 0;
      while(*(p+i) != '"'){
        if(*(p+i)== '\\' && *(p+i+1)== '"'){
          i+=2;
        }
        i++;
      }
      cur = new_token(TK_STRING, cur, p, i);
      p += i+1;
      continue;
    }
    if (isdigit(*p)){
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    if(*p == 39){ // 39 is ascii code for single quote
      p++;
      cur = new_token(TK_NUM, cur, p, 1);
      if(*p=='\\'){ // escape sequence
        p++;
        switch(*p){
          case '\\': cur->val = '\\';break;
          case '\'': cur->val = '\'';break;
          case '\"': cur->val = '\"';break;
          case 'b' : cur->val = '\b';break;
          case 'n' : cur->val = '\n';break;
          case 'r' : cur->val = '\r';break;
          case 't' : cur->val = '\t';break;
          case '0' : cur->val = '\0';break;
          default:error_at(cur->str, "Character literal contains unsupported escape sequence '\\%c'\n", *p);
        }
      }else{
        cur->val = *p;
      }
      p++;
      if(*p != 39){
        error_at(cur->str, "Character literal should end with \'\n");
      }
      p++;
      continue;
    }
    if (is_alnum(*p)){
      int i = 1;
      while(is_alnum(*(p+i))){
        ++i;
      }
      int d = 0;
      // if "if"pp
      if(d = tokenize_if_keyword_matches(p, &cur, i, "if", TK_IF)){p += d; continue;}
      // if else
      if(d = tokenize_if_keyword_matches(p, &cur, i, "else", TK_ELSE)){p += d; continue;}
      // if return
      if(d = tokenize_if_keyword_matches(p, &cur, i, "return", TK_RETURN)){p += d; continue;}
      // if while
      if(d = tokenize_if_keyword_matches(p, &cur, i, "while", TK_WHILE)){p += d; continue;}
      // if for
      if(d = tokenize_if_keyword_matches(p, &cur, i, "for", TK_FOR)){p += d; continue;}
      // variable type
      if(d = tokenize_if_keyword_matches(p, &cur, i, "int", TK_TYPE_INT)){p += d; continue;}
      if(d = tokenize_if_keyword_matches(p, &cur, i, "char", TK_TYPE_CHAR)){p += d; continue;}
      if(d = tokenize_if_keyword_matches(p, &cur, i, "void", TK_TYPE_VOID)){p += d; continue;}
      // if sizeof
      if(d = tokenize_if_keyword_matches(p, &cur, i, "sizeof", TK_SIZEOF)){p += d; continue;}
      // struct declaration
      if(d = tokenize_if_keyword_matches(p, &cur, i, "struct", TK_STRUCT)){p += d; continue;}
      // struct declaration
      if(d = tokenize_if_keyword_matches(p, &cur, i, "enum", TK_ENUM)){p += d; continue;}
      if(d = tokenize_if_keyword_matches(p, &cur, i, "typedef", TK_TYPEDEF)){p += d; continue;}
      if(d = tokenize_if_keyword_matches(p, &cur, i, "switch", TK_SWITCH)){p += d; continue;}
      if(d = tokenize_if_keyword_matches(p, &cur, i, "case", TK_CASE)){p += d; continue;}
      // NULL pointer
      if(d = tokenize_if_keyword_matches(p, &cur, i, "NULL", TK_NUM)){p += d; cur->val = 0; continue;}
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


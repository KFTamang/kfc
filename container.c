#include "kfc.h"

// error messager
void error(char* fmt, ...){
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}
// filename
char* g_filename;

// エラーの起きた場所を報告するための関数
// 下のようなフォーマットでエラーメッセージを表示する
//
// foo.c:10: x = y + + 5;
//                   ^ 式ではありません
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  // locが含まれている行の開始地点と終了地点を取得
  char *line = loc;
  while (user_input < line && line[-1] != '\n')
    line--;

  char *end = loc;
  while (*end != '\n')
    end++;

  // 見つかった行が全体の何行目なのかを調べる
  int line_num = 1;
  for (char *p = user_input; p < line; p++)
    if (*p == '\n')
      line_num++;

  // 見つかった行を、ファイル名と行番号と一緒に表示
  int indent = fprintf(stderr, "%s:%d: ", g_filename, line_num);
  fprintf(stderr, "%.*s\n", (int)(end - line), line);

  // エラー箇所を"^"で指し示して、エラーメッセージを表示
  int pos = loc - line + indent;
  fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  exit(1);
}

int round_up_to_8(int in){
  if(in%8==0){
    return in;
  }
  return in-in%8+8;
}

char* read_file(char* filename){
  g_filename = filename;
    FILE* fp = fopen(filename, "r");
    if(!fp){
        printf("File open error:%s\n", filename);
        exit(EXIT_FAILURE);
    }
    // get file size
    if(fseek(fp, 0, SEEK_END)!=0){
        printf("File move to end error\n");
        exit(EXIT_FAILURE);
    }
    long file_size = ftell(fp);

    // read file contents to buffer
    if(fseek(fp, 0, SEEK_SET)!=0){
        printf("File move to beginning error\n");
        exit(EXIT_FAILURE);
    }
    char* buf = calloc(1, file_size+2);
    fread(buf, file_size, 1, fp);

    // put string end indicator to buf
    buf[file_size]   = '\n';
    buf[file_size+1] = '\0';
    fclose(fp);
    return buf;
}


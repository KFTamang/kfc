#include "kfc.h"

char* read_file(char* filename){
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

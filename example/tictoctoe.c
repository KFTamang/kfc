
// current position
int x;
int y;

int print_data(int* data){
    int i;
    printf(" -");
    for(i=0;i<x;i=i+1){
        printf("--");
    }
    printf("↓");
    for(i=x;i<2;i=i+1){
        printf("--");
    }
    printf("-\n");
    int j;
    for(i=0;i<3;i=i+1){
        if(y==2-i){
            printf("→");
        }else{
            printf(" ");
        }
        for(j=0;j<3;j=j+1){
            printf("|");
            if(data[i*3+j] == 0){
                printf(" ");
            }else if(data[i*3+j] == 1){
                printf("●");
            }else if(data[i*3+j] == 2){
                printf("×");
            }
        }
        printf("|\n");
    }
    printf(" -------\n");
    return 0;
}

int get_input(){
    int key = getchar();
    return key;
}

int check_winner(int* data){
    int i = 0;
    int j = 0;
    for(i=0;i<3;i=i+1){
        if(data[i*3+0]==1)if(data[i*3+1]==1)if(data[i*3+2]==1){
            printf("Winner:●\n");
            exit(0);
        }
        if(data[0*3+i]==1)if(data[1*3+i]==1)if(data[2*3+i]==1){
            printf("Winner:●\n");
            exit(0);
        }
        if(data[i*3+0]==2)if(data[i*3+1]==2)if(data[i*3+2]==2){
            printf("Winner:×\n");
            exit(0);
        }
        if(data[0*3+i]==2)if(data[1*3+i]==2)if(data[2*3+i]==2){
            printf("Winner:×\n");
            exit(0);
        }
    }
    if(data[0*3+0]==1)if(data[1*3+1]==1)if(data[2*3+2]==1){
        printf("Winner:●\n");
        exit(0);
    }
    if(data[2*3+0]==1)if(data[1*3+1]==1)if(data[0*3+2]==1){
        printf("Winner:●\n");
        exit(0);
    }
    if(data[0*3+0]==2)if(data[1*3+1]==2)if(data[2*3+2]==2){
        printf("Winner:×\n");
        exit(0);
    }
    if(data[2*3+0]==2)if(data[1*3+1]==2)if(data[0*3+2]==2){
        printf("Winner:×\n");
        exit(0);
    }
    if(data[0*3+0]!=0)if(data[1*3+0]!=0)if(data[2*3+0]!=0)
    if(data[0*3+1]!=0)if(data[1*3+1]!=0)if(data[2*3+1]!=0)
    if(data[0*3+2]!=0)if(data[1*3+2]!=0)if(data[2*3+2]!=0)
    {
        printf("Draw!\n");
        exit(0);
    }
    return 1;
}

int main(){
    int i;
    int data[9];
    x = 0;
    y = 2;
    int turn = 0;
    for(i=0;i<9;i=i+1){
        data[i] = 0;
    }
    i=0;
    int continued = 1;
    int key;
    print_data(data);
    while(continued){
        i=i+1;
        if(turn == 0){
            printf("%d:●'s turn\n",i);
        }else{
            printf("%d:×'s turn\n",i);
        }
        key = get_input();
        if(key == 27){ // ESC
            continued = 0;
            printf("Game aborted\n");
        }
        if(key == 105){ // i up
            if(y<2){
                y = y+1;
            }
        }else if(key == 106){ // j left
            if(x>0){
                x = x-1;
            }
        }else if(key == 107){ // k down
            if(y>0){
                y = y-1;
            }
        }else if(key == 108){ // l right
            if(x<2){
                x = x+1;
            }
        }else if(key == 32){ // space
            if(data[(2-y)*3+x] == 0){
                if(turn == 0){
                    data[(2-y)*3+x] = 1;
                    turn = 1;
                }else{
                    data[(2-y)*3+x] = 2;
                    turn = 0;
                }
            }
        }
        printf("Please input key\n");
        printf("i : up\n");
        printf("j : left\n");
        printf("l : right\n");
        printf("k : down\n");
        printf("ESC: abort game\n");
        print_data(data);
        check_winner(data);
    }
    print_data(data);
    return 11;
}

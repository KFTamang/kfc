int return_3(){ int a; a = 3; return a; } 
int return_5(){ int a; a = 5; return a; } 
int return_9(){ return 9; } 
int return_7(){ return 7; } 
int return_x(int x){ return x; } 
int add_x_y(int x, int y){ return x+y; } 
int add(int x, int y){ return x+y;}
int add_x_y_z(int x, int y, int z){ return x+y+z; } 
int add_x_y_z_minus_v(int x, int y, int z, int v){ return x+y+z-v; } 
int add_x_y_z_times_v(int x, int y, int z, int v){ return (x+y+z)*v; } 
int fib(int x){  
    if(x<2){
        return 1;
    }else{
        return fib(x-1) + fib(x-2);
    }
}

int test2(){int a; a = 5; return a;}
int test3(){ int a; a = 5; int b; b = 9; a = a + 1; return a+b;}
int test4(){ return return_9(); }
int test5(){ int a; a = return_7(); return a; }
int test6(){ int a; a = 9; return a; }
int test7(){ return return_5(); }
int test8(){ int b; int c; int d; b = return_3(); return b; }
int test9(){ int b; b = return_3(); return b; }
int test10(){ return 7; }
int test11(){ return return_7(0); }
int test12(){ return return_x(9); }
int test13(){ int a; a = 9; return return_x(a+7); }
int test14(){ return add_x_y(6,10); }
int test15(){ if(add(5,3)==8){ return 7; }else{ return 2; }}
int test16(){ return add_x_y_z(6,10,7); }
int test17(){ return add_x_y_z_minus_v(6,10,7,5); }
int test18(){ return add_x_y_z_times_v(6,10,4,2); }
int test19(){ if(return_x(-2) == -2){ return 3; }else{ return 5; } }
int test20_(){ int x; int* y; x = 5; y = &x; return *y; } 
int test20(){ return test20_(); }
int test21_(){ int x; int y; int* z; x = 3; y = 7; z = &x - 1; return *z; } 
int test21(){ return test21_(); }
int test22(){ int x; int *y; int **z; return 11; }
int test23(){ int x; int *y; y = &x; *y = 3; if(x == 3){ return 5; }else{ return 7; } }
int test24(){ int a; a = 3; int b; b = 5; int c; c = 7; int *p; p = &c; return *(p+1); }
int test25(){ int *p; alloc4(&p, 1, 3, 5, 7); int *q; q = p + 3; return *q; }
int test26(){ int *p; alloc4(&p, 1, 3, 5, 7); int *q; q = p + 2*1; return *q; }
int test27(){ int *p; alloc4(&p, 1, 3, 5, 7); int *q; q = p + 2/2; return *q; }
int test28(){ int *p; alloc4(&p, 1, 3, 5, 7); int *q; q = p + 3; q = q - 1; return *q; }
int test29(){ int *p; alloc4(&p, 1, 3, 5, 7); int *q; q = p + 3 - 2; return *q; }
int test30(){ int x; sizeof(x); return 0; }
int test31(){ int x; return sizeof(x); }
int test32(){ int x; return sizeof x; }
int test33(){ int* x; return sizeof x; }
int test34(){ int x; x = 7; return sizeof(x+4); }
int test35(){ int x[10]; return 7; }
int test36(){ int x[10]; return sizeof(x); }
int test37(){ int a[3]; *(a + 2) = 4; return *(a + 2); }
int test38(){ int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p; }
int test39(){ int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *(p+1); }
int test40(){ int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p+1); }
int test41(){ int a[3]; *a = 1; int x; x = 2; *(a + 2) = 4; return *(a + 2); }
int test42(){ int x[10]; *(x + sizeof(x)-1) = 10; return *(x + sizeof(x)-1); }
int test43(){ int x[10]; int y[20]; *(x + sizeof(x)-1) = 10; *(y + sizeof(y)-3) = 9; return *(x + sizeof(x)-1) + *(y + sizeof(y)-3); }
int test44(){ int x[10]; x[9] = 10; return x[9];}
int test45(){ int x[10];int y[2]; x[3]=1; y[x[3]]=7;return y[1];}
int test46(){ int x[10];x[3]=9; return 3[x];}
int test47(){ int x[10];int i;i=9;x[i]=17; return x[i];}
int global_var;
int test48(){ global_var = 2;return global_var;}
int g;
int test49(){g=2;int local; local = g+4; return local+g;}
int global_variable_dayo;
int test50(){g=2;int global_variable_dayo; global_variable_dayo = 9; return g*global_variable_dayo;}
int test51(){g=2;int g; g = 4; return g;}
int test52(){char a;a = 128;return a;}
int test53(){char a[10];a[0]=7;return a[0];}
int test54(){char a[10];a[0]=0;a[1]=1;a[2]=2;a[3]=3;a[4]=4;a[5]=5;a[6]=6;a[7]=7;a[8]=8;a[9]=9;return a[9];}
int test55(){char a[10];int i;i=8;a[i]=i;return a[i];}
int test56(){char a[10];char i;for(i=0;i<9;i=i+1){a[i]=i;}return a[5];}
int test57(){char* a; a = "hoge";return a[0];}
int test58(){char* a; a = "hoge";return a[1];}
int test59(){char* a; a = "hoge";return a[3];}
int test60(){char* a; a = "hogefugamogepoyO";return a[15];}
int test61(){print_error("hogefugamogepoyO\n");return 9;}
int test62(){int a = 11;return a;}
int test63(){int hoge = 7;int poyo = return_3(); return hoge*poyo;}
int test64(){struct{ int a;int b;char c; char* hoge;} x; return 9;}
int test65(){struct{ int a;int b;char hoge; char* bbagepoyo;} x; return sizeof(x);}
int test66(){struct{ int a;int b[10];char hoge; char* bbagepoyo;} x; return sizeof(x);}
int test67(){struct{ int a; struct{int poyo; char mogera[8];} rec_struct[2];} x; return sizeof(x);}
int test68(){struct{ int a;} x; x.a = 4; return x.a;}
int test69(){struct{ int a; int b;} x; x.a = 4; x.b = 9; return x.a+x.b;}
int test70(){struct{ int a; int b;} x[2]; x[0].a = 4; x[1].b = 9; return x[0].a+x[1].b;}
int test71(){int i=0;while(i<10000){i=i+1; if(i==100){return 38;}}return 25;}
int test72(){struct humi{int fuga; char tero;} humi_imp; struct humi hoge;hoge.fuga = 9;return hoge.fuga;}
int test73(){struct humi{int fuga; char tero;}; struct humi humi_imp2; humi_imp2.tero = 11; humi_imp2.fuga = 23; return humi_imp2.tero+humi_imp2.fuga;}
int test74(){struct list_hoge{int value; struct values{int val1;int value2;};}; return sizeof (struct list_hoge);}
int test75(){
    struct linked_list;
    struct linked_list{
        int val;
    };
    struct linked_list head;
    head.val = 8;
    return head.val;
}
int test76(){
    struct linked_list{
        int val;
        struct linked_list* next;
    }; 
    struct linked_list head;
    struct linked_list tail;
    head.val = 2;
    head.next = &tail;
    tail.val = 3;
    return head.val+tail.val;
}
int test77(){
    struct test * hoge;
    struct test{char* string;};
    struct test obj;
    hoge = &obj;
    (*hoge).string = 8;
    return (*hoge).string;
}
struct g_list{int hoge; struct g_list* next;};
int sum_list(struct g_list* hoge){
    if((*hoge).next == 0){
        return (*hoge).hoge;
    }
    return (*hoge).hoge + sum_list((*hoge).next);
}
int test78(){
    struct g_list list0;
    struct g_list list1;
    struct g_list list2;
    struct g_list list3;
    list0.hoge = 0;
    list1.hoge = 1;
    list2.hoge = 2;
    list3.hoge = 4;
    list0.next = &list1;
    list1.next = &list2;
    list2.next = &list3;
    list3.next = 0;
    return sum_list(&list0);
}
int test79(){
    struct humi {int poyo; int arr;int* pointer;} fuga;
    struct humi * wao = &fuga;
    wao->poyo = 9;
    wao->arr = 7;
    wao->pointer = &wao;
    return wao->poyo + wao->arr;
}
int test80(){
    enum {NUM1, NUM2, NUM3};
    return 7;
}
int test81(){
    enum {NUM1, NUM2, NUM3};
    return NUM1+NUM2+NUM3;
}
int test82(){
    enum {HOGE, FUGA, POYO} enum_test;
    enum_test = FUGA;
    return enum_test;
}
int test83(){
    typedef int my_INT;
    my_INT hoge = 9;
    return hoge;
}
int test84(){
    typedef struct{int hoge;char poyo;} my_struct;
    my_struct hoge;
    hoge.hoge = 7;
    return hoge.hoge;
}
int test85(){
    return (1+3==4)&&(2*1!=3);
}
int test86(){
    return (1+3==4)&&(2*1==3);
}
int test87(){
    if(5>2 && 2/2==1){
        return 9;
    }else{
        return 11;
    }
}
int test88(){
    return (1+3!=4)||(2*1!=3);
}
int test89(){
    return (1+3!=4)||(2*1>3);
}
int test90(){
    if(5<2 || 2/2!=1){
        return 9;
    }else{
        return 11;
    }
}
int test91(){
    return !0;
}
int test92(){
    return !1;
}
int test93(){
    if(!(2==2)){
        return 9;
    }else{
        return 7;
    }
}
int array[10];
int global_array_test(){
    int i=0;
    for(i=0;i<10;i=i+1){
        array[i] = i;
    }
    return array[9];
}
int test94(){
    int i = 1;
    ++i;
    return i;
}
int test95(){
    int i = 1;
    ++i;
    ++i;
    return i;
}
int test96(){
    int i = 3;
    int o = 7;
    o = ++i;
    return o;
}
int test97(){
    int j = 0;
    for(int i=0;i<10;++i){
        ++j;
    }
    return j;
}
int test98(){
    enum {HOGE, FUGA, POYO} enum_test;
    enum_test = FUGA;
    switch(enum_test){
        case HOGE: return 9;
        case FUGA: return 7;
        case POYO: return 5;
    }
    return 7;
}

int test99(){
    int hoge = 1;
    int ret = 9;
    switch (hoge){
    case 0:
        ret = 1;
    case 1:
        ret = 2;
    case 2:
        ret = 3;
    }
    return ret;
}

int test100(){
    int hoge = 1;
    int ret = 9;
    switch (hoge){
    case 0:
        ret = 1;
        break;
    case 1:
        ret = 2;
        break;
    case 2:
        ret = 3;
        break;
    }
    return ret;
}

int test101(){
    int hoge = 0;
    int ret = 9;
    switch (hoge){
    case 0:
        ret = 1;
    case 1:
        ret = 2;
    case 2:
        ret = 3;
        break;
    }
    return ret;
}

int test102(){
    int i = 0;
    for(i=0;i<10;++i){
        if(i==5){
            break;
        }
    }
    return i;
}

int test103(){
    int hoge = 13;
    int ret = 9;
    switch (hoge){
    case 0:
        ret = 1;
    case 1:
        ret = 2;
    case 2:
        ret = 3;
        break;
    default:
        ret = 123;
    }
    return ret;
}

int test104(){
    int hoge = 11;
    hoge += 2;
    return hoge;
}

int test105(){
    int i = 3;
    for(;i<10;++i){
        continue;
    }
    return i;
}

int test106(){
    int i = 0;
    while(i<14){
        ++i;
        continue;
        return i;
    }
    return i;
}

int test107(){
    int i = 7;
    while(i<14){
        break;
        return i;
    }
    return i;
}

int assert(int expected, int actual, char* code){
    if(expected == actual){
        return 0;
    }else{
        print_error("Test failed\n%s\n", code);
        print_error("expected : %d but got %d\n", expected, actual);
        printf("int main(){\n");
        printf("%s\n",code);
        printf("return 0;\n}\n");
        exit(1);
    }
}
int main(){
    assert( 5 , test2(),"int test2(){int a; a = 5; return a;}");
    assert( 15 , test3(),"int main(){ int a; a = 5; int b; b = 9; a = a + 1; return a+b;}");
    assert( 9 , test4()," int return_9(){ return 9; } int test4(){ return return_9(); }");
    assert( 7 , test5()," int return_7(){ return 7; } int test5(){ int a; a = return_7(); return a; }");
    assert( 9 , test6()," int return_9(){ return 9; } int test6(){ int a; a = 9; return a; }");
    assert( 5 , test7()," int return_5(){ int a; a = 5; return a; } int test7(){ return return_5(); }");
    assert( 3 , test8()," int return_3(){ int a; a = 3; return a; } int test8(){ int b; int c; int d; b = return_3(); return b; }");
    assert( 3 , test9()," int return_3(){ int a; a = 3; return a; } int test9(){ int b; b = return_3(); return b; }");
    assert( 7 , test10()," int return_7(int x){ return 7; } int test10(){ return 7; }");
    assert( 7 , test11()," int return_7(int x){ return 7; } int test11(){ return return_7(0); }");
    assert( 9 , test12()," int return_x(int x){ return x; } int test12(){ return return_x(9); }");
    assert( 16 , test13()," int return_x(int x){ return x; } int test13(){ int a; a = 9; return return_x(a+7); }");
    assert( 1 , fib(0),"int fib(int x){ if(x<2){ return 1; }else{ return fib(x-1) + fib(x-2); }}");
    assert( 1 , fib(1),"int fib(int x){ if(x<2){ return 1; }else{ return fib(x-1) + fib(x-2); }}");
    assert( 2 , fib(2),"int fib(int x){ if(x<2){ return 1; }else{ return fib(x-1) + fib(x-2); }}");
    assert( 3 , fib(3),"int fib(int x){ if(x<2){ return 1; }else{ return fib(x-1) + fib(x-2); }}");
    assert( 8 , fib(5),"int fib(int x){ if(x<2){ return 1; }else{ return fib(x-1) + fib(x-2); }}");
    assert( 89 , fib(10),"int fib(int x){ if(x<2){ return 1; }else{ return fib(x-1) + fib(x-2); }}");
    assert( 16 , test14()," int add_x_y(int x, int y){ return x+y; } int test14(){ return add_x_y(6,10); }");
    assert( 7 , test15(),"int add(int x, int y){ return x+y;}int test15(){ if(add(5,3)==8){ return 7; }else{ return 2; }}");
    assert( 23 , test16()," int add_x_y_z(int x, int y, int z){ return x+y+z; } int test16(){ return add_x_y_z(6,10,7); }");
    assert( 18 , test17()," int add_x_y_z_minus_v(int x, int y, int z, int v){ return x+y+z-v; } int test17(){ return add_x_y_z_minus_v(6,10,7,5); }");
    assert( 40 , test18()," int add_x_y_z_times_v(int x, int y, int z, int v){ return (x+y+z)*v; } int test18(){ return add_x_y_z_times_v(6,10,4,2); }");
    assert( 3 , test19()," int return_x(int x){ return x; } int test19(){ if(return_x(-2) == -2){ return 3; }else{ return 5; } }");
    assert( 5 , test20(),"int test(){ int x; int* y; x = 5; y = &x; return *y; } int test20(){ return test(); } ");
    assert( 7 , test21(),"int test(){ int x; int y; int* z; x = 3; y = 7; z = &x - 1; return *z; } int test21(){ return test(); } ");
    assert( 11 , test22()," int test22(){ int x; int *y; int **z; return 11; } ");
    assert( 5 , test23()," int test23(){ int x; int *y; y = &x; *y = 3; if(x == 3){ return 5; }else{ return 7; } } ");
    assert( 5 , test24()," int test24(){ int a; a = 3; int b; b = 5; int c; c = 7; int *p; p = &c; return *(p+1); } ");
    assert( 7 , test25()," int test25(){ int *p; alloc4(&p, 1, 3, 5, 7); int *q; q = p + 3; return *q; } ");
    assert( 5 , test26()," int test26(){ int *p; alloc4(&p, 1, 3, 5, 7); int *q; q = p + 2*1; return *q; } ");
    assert( 3 , test27()," int test27(){ int *p; alloc4(&p, 1, 3, 5, 7); int *q; q = p + 2/2; return *q; } ");
    assert( 5 , test28()," int test28(){ int *p; alloc4(&p, 1, 3, 5, 7); int *q; q = p + 3; q = q - 1; return *q; }");
    assert( 3 , test29()," int test29(){ int *p; alloc4(&p, 1, 3, 5, 7); int *q; q = p + 3 - 2; return *q; }");
    assert( 0 , test30()," int test30(){ int x; sizeof(x); return 0; } ");
    assert( 8 , test31()," int test31(){ int x; return sizeof(x); } ");
    assert( 8 , test32()," int test32(){ int x; return sizeof x; } ");
    assert( 8 , test33()," int test33(){ int* x; return sizeof x; } ");
    assert( 8 , test34()," int test34(){ int x; x = 7; return sizeof(x+4); } ");
    assert( 7 , test35()," int test35(){ int x[10]; return 7; } ");
    assert( 80 , test36()," int test36(){ int x[10]; return sizeof(x); } ");
    assert( 4 , test37()," int test37(){ int a[3]; *(a + 2) = 4; return *(a + 2); }");
    assert( 1 , test38()," int test38(){ int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p; }");
    assert( 2 , test39()," int test39(){ int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *(p+1); }");
    assert( 3 , test40()," int test40(){ int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p+1); }");
    assert( 4 , test41()," int test41(){ int a[3]; *a = 1; int x; x = 2; *(a + 2) = 4; return *(a + 2); }");
    assert( 10 , test42()," int test42(){ int x[10]; *(x + sizeof(x)-1) = 10; return *(x + sizeof(x)-1); }");
    assert( 19 , test43()," int test43(){ int x[10]; int y[20]; *(x + sizeof(x)-1) = 10; *(y + sizeof(y)-3) = 9; return *(x + sizeof(x)-1) + *(y + sizeof(y)-3); }");
    assert( 10 , test44(),"int test44(){ int x[10]; x[9] = 10; return x[9];}");
    assert( 7 , test45(),"int test45(){ int x[10];int y[2]; x[3]=1; y[x[3]]=7;return y[1];}");
    assert( 9 , test46(),"int test46(){ int x[10];x[3]=9; return 3[x];}");
    assert( 17 , test47(),"int test47(){ int x[10];int i;i=9;x[i]=17; return x[i];}");
    assert( 2 , test48(),"int global_var;int test48(){ global_var = 2;return global_var;}");
    assert( 8 , test49(),"int g;int test49(){g=2;int local; local = g+4; return local+g;}");
    assert( 18 , test50(),"int g;int global_variable_dayo;int test50(){g=2;int global_variable_dayo; global_variable_dayo = 9; return g*global_variable_dayo;}");
    assert( 4 , test51(),"int g;int test51(){g=2;int g; g = 4; return g;}");
    assert( 128 , test52(),"int test52(){char a;a = 128;return a;}");
    assert( 7 , test53(),"int test53(){char a[10];a[0]=7;return a[0];}");
    assert( 9 , test54(),"int test54(){char a[10];a[0]=0;a[1]=1;a[2]=2;a[3]=3;a[4]=4;a[5]=5;a[6]=6;a[7]=7;a[8]=8;a[9]=9;return a[9];}");
    assert( 8 , test55(),"int test55(){char a[10];int i;i=8;a[i]=i;return a[i];}");
    assert( 5 , test56(),"int test56(){char a[10];char i;for(i=0;i<9;i=i+1){a[i]=i;}return a[5];}");
    assert( 104, test57(),"");
    assert( 111, test58(),"");
    assert( 101, test59(),"");
    assert( 79,  test60(),"");
    assert( 9,   test61(),"");
    // comment test
    /* block comment */
    /* comment /*
    over 
    lines; */
    assert(11, test62(), "int main(){int a = 11;return a;}");
    assert(21, test63(), "int main(){int hoge = 7;int poyo = return_3(); return hoge*poyo;}");
    assert(9,  test64(), "int main(){struct{ int a;int b;} x; return 9;}");
    assert(25, test65(), "struct{int a;int b;char hoge; char* bbagepoyo;} x; return sizeof(x);");
    assert(97, test66(), "struct{int a;int b[10];char hoge; char* bbagepoyo;} x; return sizeof(x);");
    assert(40, test67(), "struct{int a; struct{int poyo; char mogera[8];} rec_struct[2];} x; return sizeof(x);");
    assert(4,  test68(), "struct{int a;} x; x.a = 4; return x.a;");
    assert(13, test69(), "struct{ int a; int b;} x; x.a = 4; x.b = 9; return x.a+x.b;");
    assert(13, test70(), "struct{ int a; int b;} x[2]; x[0].a = 4; x[1].b = 9; return x[0].a+x[1].b;");
    assert(38, test71(), "int i=0;while(i<10000){i=i+1; if(i==100){return 38;}}return 25;");
    // struct with name
    assert(9, test72(), "struct humi{int fuga; char tero;} humi_imp; struct humi hoge;hoge.fuga = 9;return hoge.fuga;");
    assert(34, test73(), "struct humi{int fuga; char tero;}; struct humi humi_imp2; humi_imp2.tero = 11; humi_imp2.fuga = 23; return humi_imp2.tero+humi_imp2.fuga;");
    // struct hoyo{int arr[4];char* str;};
    // // typedef
    // typedef struct humi HUMI;
    // typedef struct test{int hoge;}TEST;
    // struct test poyo;
    // struct test foo;
    print_error("Test passed\n");
    return 0;
}

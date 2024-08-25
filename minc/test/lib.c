#include <stdlib.h>

// このコードは本コンパイラによってコンパイルされず、GCC等がコンパイルする
int assert(int actual, int expect)
{
    if (actual == expect)
        return 0;
    exit(1);
}


// 関数呼び出しテスト用
int sum(int a, int b, int c, int d, int e, int f)
{
    return a + b + c + d + e + f;
}
int hoge()
{
    return 123;
}

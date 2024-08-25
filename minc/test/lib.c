#include <stdlib.h>

// このコードは本コンパイラによってコンパイルされず、GCC等がコンパイルする
int assert(int actual, int expect)
{
    if (actual == expect)
        return 0;
    exit(1);
}

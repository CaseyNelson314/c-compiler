#include <stdlib.h>

// このコードは本コンパイラによってコンパイルされず、GCC等がコンパイルする

void call_exit(int code)
{
    exit(code);
}
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "minc.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    user_input = argv[1];
    token = tokenize(argv[1]);
    Node *node = parse();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  push rbp\n"); // ベースポインタを保持
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n"); // 領域を確保

    while (node)
    {
        gen(node);
        printf("  pop rax\n");
        node = node->next;
    }

    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n"); // ベースポインタを復元
    printf("  ret\n");

    return 0;
}

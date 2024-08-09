#include "minc.h"

#include <stdio.h>

void gen(Node *node)
{
    if (node->kind == ND_NUM)
    {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind)
    {
    case ND_ADD: // +
        printf("  add rax, rdi\n");
        break;
    case ND_SUB: // -
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL: // *
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV: // /
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;

    case ND_EQ: // ==
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");       // al(arxの下部8bit) に比較結果をセット
        printf("  movzb rax, al\n"); // 上部56bitをゼロクリア (seteは8bitレジスタにのみセット可能なのでこのような書き方となる)
        break;
    case ND_NE: // !=
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT: // <
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE: // <=
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;

    default:
        error("error");
    }

    printf("  push rax\n");
}

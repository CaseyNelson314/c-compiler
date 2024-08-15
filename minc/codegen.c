#include "minc.h"

#include <stdio.h>

static void gen_lvalue(Node *node)
{
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("  mov rax, rbp\n");              // スタックベースポインタを取得
    printf("  sub rax, %d\n", node->offset); // オフセット分スタックポインタを減算
    printf("  push rax\n");                  // 得られたアドレス値を保存
}

static int label_counter;

// ASTをたどり、アセンブリを吐く
// 評価結果をスタックにプッシュするアセンブリを生成する
void gen(Node *node)
{
    switch (node->kind)
    {
    case ND_ASSIGN:
        gen_lvalue(node->lhs);
        gen(node->rhs);
        printf("  pop rdi\n");        // 右辺値の値
        printf("  pop rax\n");        // 左辺値のアドレス
        printf("  mov [rax], rdi\n"); // 代入
        printf("  push rdi\n");       // 代入は式であるため、値を返す必要がある
        return;

    case ND_LVAR: // ローカル変数
        gen_lvalue(node);
        printf("  pop rax\n");        // スタックポインタのアドレスを取得
        printf("  mov rax, [rax]\n"); // アドレス先の値をロード
        printf("  push rax\n");       // 値をプッシュ
        return;

    case ND_RETURN:
        gen(node->lhs); // 左辺にreturnに続く式が来る 右辺はNULL
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n"); // スタックポインタを復元
        printf("  pop rbp\n");      // ベースポインタを復元
        printf("  ret\n");
        return;

    case ND_IF:
        if (node->else_stmt) // if .. else ..
        {
            gen(node->if_state);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lif.else%d\n", label_counter); // 条件式が0である場合elseへ飛ばす
            gen(node->if_stmt);
            printf("  jmp .Lif.end%d\n", label_counter);
            printf(".Lif.else%d:\n", label_counter);
            gen(node->else_stmt);
            printf(".Lif.end%d:\n", label_counter);
        }
        else // if ..
        {
            gen(node->if_state);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .Lif.end%d\n", label_counter); // 条件式が0である場合終了
            gen(node->if_stmt);
            printf(".Lif.end%d:\n", label_counter);
        }
        label_counter++;
        return;

    case ND_WHILE:
        printf(".Lwhile.begin%d:\n", label_counter);

        gen(node->while_state);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lwhile.end%d\n", label_counter);

        gen(node->while_stmt);
        printf("  pop rax\n"); // 評価値を捨てる
        printf("  jmp .Lwhile.begin%d\n", label_counter);

        printf(".Lwhile.end%d:\n", label_counter);

        printf("  push rax\n");
        label_counter++;
        return;

    case ND_FOR:
        if (node->for_init)
        {
            gen(node->for_init);
            printf("  pop rdi\n");
        }

        printf(".Lfor.begin%d:\n", label_counter); // ループ開始

        if (node->for_cond)
        {
            gen(node->for_cond);
            printf("  pop rdi\n");
            printf("  cmp rdi, 0\n");
            printf("  je .Lfor.end%d\n", label_counter); // 条件不成立で終了へジャンプ
        }

        gen(node->for_stmt);
        printf("  pop rax\n");

        if (node->for_loop)
        {
            gen(node->for_loop);
            printf("  pop rdi\n");  // for_stmtの評価値を残しておきたいのでrdiに捨てる
        }

        printf("  jmp .Lfor.begin%d\n", label_counter); // 先頭へ戻る
        printf(".Lfor.end%d:\n", label_counter);        // ループ終了

        printf("  push rax\n");

        label_counter++;
        return;

    case ND_NUM:
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

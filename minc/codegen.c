//
//    コードジェネレーター
//
//    評価後値をraxレジスタに保持するアセンブリを生成する
//

#include "minc.h"

#include <stdio.h>
#include <stdarg.h>

static void println(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
}

static void gen_lvalue(Node *node)
{
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    println("  mov rax, rbp");              // スタックベースポインタを取得
    println("  sub rax, %d", node->offset); // オフセット分スタックポインタを減算
}

static int gen_unique()
{
    static int count;
    return count++;
}

// ASTをたどり、アセンブリを吐く
void gen(Node *node)
{
    switch (node->kind)
    {
    case ND_ASSIGN:
        // 左辺値を rdi
        gen_lvalue(node->lhs);
        println("  push rax");

        // 右辺値を rax
        gen(node->rhs);

        println("  pop rdi");

        // 代入
        println("  mov [rdi], rax");
        return;

    case ND_LVAR:                    // ローカル変数
        gen_lvalue(node);            // -> rax
        println("  mov rax, [rax]"); // アドレス先の値をロード
        return;

    case ND_RETURN:
        gen(node->return_expr);
        println("  mov rsp, rbp"); // スタックポインタを復元
        println("  pop rbp");      // ベースポインタを復元
        println("  ret");
        return;

    case ND_IF:
    {
        int count = gen_unique();
        if (node->else_stmt) // if .. else ..
        {
            gen(node->if_state);
            println("  cmp rax, 0");
            println("  je .Lif.else%d", count); // 条件式が0である場合elseへ飛ばす
            gen(node->if_stmt);
            println("  jmp .Lif.end%d", count);
            println(".Lif.else%d:", count);
            gen(node->else_stmt);
            println(".Lif.end%d:", count);
        }
        else // if ..
        {
            gen(node->if_state);
            println("  cmp rax, 0");
            println("  je .Lif.end%d", count); // 条件式が0である場合終了
            gen(node->if_stmt);
            println(".Lif.end%d:", count);
        }
        return;
    }

    case ND_WHILE:
    {
        int count = gen_unique();
        println(".Lwhile.begin%d:", count);

        gen(node->while_state);
        println("  cmp rax, 0");
        println("  je .Lwhile.end%d", count);

        gen(node->while_stmt);
        println("  jmp .Lwhile.begin%d", count);
        println(".Lwhile.end%d:", count);
        return;
    }

    case ND_FOR:
    {
        int count = gen_unique();

        if (node->for_init)
        {
            gen(node->for_init);
        }

        println(".Lfor.begin%d:", count); // ループ開始

        if (node->for_cond)
        {
            gen(node->for_cond);
            println("  cmp rax, 0");
            println("  je .Lfor.end%d", count); // 条件不成立で終了へジャンプ
        }

        gen(node->for_stmt);

        if (node->for_loop)
        {
            gen(node->for_loop);
        }

        println("  jmp .Lfor.begin%d", count); // 先頭へ戻る
        println(".Lfor.end%d:", count);        // ループ終了

        return;
    }

    case ND_BLOCK:
        for (int i = 0; i < node->block_len; ++i)
        {
            gen(node->block[i]);
        }
        return;

    case ND_FUNC_CALL:
    {
        // 第1~第6引数 : RDI, RSI, RDX, RCX, R8, R9
        const char *arg_reg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
        int index = 0;

        for (Node *cur = node->func_args; cur; cur = cur->next)
        {
            gen(cur);
            println("  mov %s, rax", arg_reg[index]);
            ++index;
        }

        println("  call %.*s", node->id_len, node->id_name);

        return;
    }

    case ND_NUM:
        println("  mov rax, %d", node->val);

        return;
    }

    gen(node->rhs); // -> rdi
    println("  push rax");
    gen(node->lhs); // -> rax
    println("  pop rdi");

    switch (node->kind)
    {
    case ND_ADD: // +
        println("  add rax, rdi");
        break;
    case ND_SUB: // -
        println("  sub rax, rdi");
        break;
    case ND_MUL: // *
        println("  imul rax, rdi");
        break;
    case ND_DIV: // /
        println("  cqo");
        println("  idiv rdi");
        break;

    case ND_EQ: // ==
        println("  cmp rax, rdi");
        println("  sete al");       // al(arxの下部8bit) に比較結果をセット
        println("  movzb rax, al"); // 上部56bitをゼロクリア (seteは8bitレジスタにのみセット可能なのでこのような書き方となる)
        break;
    case ND_NE: // !=
        println("  cmp rax, rdi");
        println("  setne al");
        println("  movzb rax, al");
        break;
    case ND_LT: // <
        println("  cmp rax, rdi");
        println("  setl al");
        println("  movzb rax, al");
        break;
    case ND_LE: // <=
        println("  cmp rax, rdi");
        println("  setle al");
        println("  movzb rax, al");
        break;

    default:
        error("error");
    }
}

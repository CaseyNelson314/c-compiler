#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

char *user_input;

void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

///////////////////////////////////////////////////////////////////////////
//
//    トークナイザ
//

typedef enum
{
    TK_RESERVED, // 記号
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struct Token
{
    TokenKind kind;
    Token *next;
    int val;
    char *str;
};

Token *token;

// トークンを読み、期待した記号であるとき次へ進め真を返す
bool consume(char op)
{
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

// トークンを読み、期待した記号であるとき次へ進め、それ以外の場合エラーを報告する
void expect(char op)
{
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "'%C'ではありません", op);
    token = token->next;
}

// トークンを読み、数である場合それを返す。数でない場合エラーを報告する
int expect_number()
{
    if (token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

// トークンを読み、末端である場合真を返す
bool at_eof()
{
    return token->kind == TK_EOF;
}

// トークンを作成し、cur につなげる
Token *new_token(TokenKind kind, Token *cur, char *str)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// p をトークナイズする
Token *tokenize(char *p)
{
    Token head; // cur ポインタを while 内で使えると便利なので、インスタンスを一時的に用意する
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {

        // 空白文字スキップ
        if (isspace(*p))
        {
            ++p;
            continue;
        }

        // + -
        if (*p == '+' || *p == '-')
        {
            cur = new_token(TK_RESERVED, cur, p);
            ++p;
            continue;
        }

        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10); // strtol は p を数字列の後ろへインクリメントする効果がある
            continue;
        }

        error_at(p, "トークナイズ出来ません");
    }

    new_token(TK_EOF, cur, p);

    return head.next;
}

///////////////////////////////////////////////////////////////////////////
//
//    パーサー
//

typedef enum
{
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // 整数
} NodeKind;

typedef struct Node Node;

struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}


Node *expr();

// primary = num | "(" expr ")"
Node *primary()
{
    if (consume('('))
    {
        Node *node = expr();
        expect(')');
    }

    return new_node_num(expect_number());
}

// mul = primary ("*" primary | "/" primary)*
Node *mul()
{
    Node *node = primary();

    for (;;)
    {
        if (consume('*'))
            node = new_node(ND_MUL, node, primary());
        else if (consume('/'))
            node = new_node(ND_DIV, node, primary());
        else
            return node;
    }
}

// expr= mul ("+" mul | "-" mul)*
Node *expr()
{
    Node *node = mul();

    for (;;)
    {
        if (consume('+'))
            node = new_node(ND_ADD, node, mul());
        else if (consume('-'))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    user_input = argv[1];
    token = tokenize(argv[1]);
    

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    // printf("  mov rax, %d\n", expect_number());

    while (!at_eof())
    {
        // if (consume('+'))
        // {
        //     printf("  add rax, %d\n", expect_number());
        //     continue;
        // }

        // if (consume('-'))
        // {
        //     printf("  sub rax, %d\n", expect_number());
        //     continue;
        // }

        // error("予期しないエラー");
    }

    printf("  ret\n");

    return 0;
}

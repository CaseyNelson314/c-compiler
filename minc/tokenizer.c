///////////////////////////////////////////////////////////////////////////
//
//    トークナイザ
//

#include "minc.h"

#include <string.h>
#include <ctype.h>

Token *token;

// トークンを読み、期待した記号であるとき次へ進め真を返す
bool consume(char *op)
{
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

// トークンを読み、識別子であるとき次へ進め識別子のトークンを返す
Token *consume_ident()
{
    if (token->kind != TK_IDENT)
        return NULL;
    Token *curr = token;
    token = token->next;
    return curr;
}

// トークンを読み、期待した記号であるとき次へ進め、それ以外の場合エラーを報告する
void expect(char *op)
{
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, "'%s'ではありません", op);
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
Token *new_token(TokenKind kind, Token *cur, char *str, size_t len)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

bool startswith(char *src, char *tar)
{
    return strncmp(src, tar, strlen(tar)) == 0;
}

// p をトークナイズする
Token *tokenize(char *p)
{
    Token head; // cur ポインタを while 内で使えると便利なので、インスタンスを一時的に用意する
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {

        // space
        if (isspace(*p))
        {
            ++p;
            continue;
        }

        // multi letter punctuator
        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">="))
        {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        // single letter punctuator
        if (strchr("+-*/=()<>;", *p))
        {
            cur = new_token(TK_RESERVED, cur, p, 1);
            p += 1;
            continue;
        }

        // single letter identifier
        if ('a' <= *p && *p <= 'z')
        {
            cur = new_token(TK_IDENT, cur, p, 1);
            p += 1;
            continue;
        }

        // integer
        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10); // strtol は p を数字列の後ろへインクリメントする効果がある
            continue;
        }

        error_at(p, "トークナイズ出来ません");
    }

    new_token(TK_EOF, cur, p, 0);

    return head.next;
}

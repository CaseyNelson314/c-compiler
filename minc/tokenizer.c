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

static bool startswith(char *src, char *tar)
{
    return strncmp(src, tar, strlen(tar)) == 0;
}

static bool is_ident_head(char c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

static bool is_ident_body(char c)
{
    return is_ident_head(c) || ('0' <= c && c <= '9');
}

static int is_ident(char *op)
{
    if (!is_ident_head(*op))
        return 0;
    else
        ++op;

    int len = 1;
    while (is_ident_body(*op))
    {
        ++len;
        ++op;
    }
    return len;
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

        // identifier
        int id_len = is_ident(p);
        if (id_len)
        {
            cur = new_token(TK_IDENT, cur, p, id_len);
            p += id_len;
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
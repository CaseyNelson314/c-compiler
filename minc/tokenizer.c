///////////////////////////////////////////////////////////////////////////
//
//    トークナイザ
//

#include "minc.h"

#include <string.h>
#include <ctype.h>

Token *token;

bool equal(Token *tok, char *op)
{
    return tok->kind == TK_PUNCT &&
           strlen(op) == tok->len &&
           strncmp(tok->str, op, tok->len) == 0;
}

bool equal_kind(TokenKind kind)
{
    return token->kind == kind;
}

// トークンを読み、期待した記号であるとき次へ進め真を返す
bool skip(char *op)
{
    if (equal(token, op))
    {
        token = token->next;
        return true;
    }
    return false;
}

// トークンを読み、期待したトークンであるときトークンを返し次へ進める それ以外はNULL
Token *consume_kind(TokenKind kind)
{
    if (token->kind == kind)
    {
        Token *cur = token;
        token = token->next;
        return cur;
    }
    return NULL;
}

// トークンを読み、期待した記号であるとき次へ進め、それ以外の場合エラーを報告する
void expect(char *op)
{
    if (equal(token, op))
        token = token->next;
    else
        error_at(token->str, "'%s'ではありません", op);
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

Token *expect_ident()
{
    if (token->kind != TK_IDENT)
        error_at(token->str, "識別子ではありません");
    Token *cur = token;
    token = token->next;
    return cur;
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
        // space newline
        if (isspace(*p) || *p == '\n' || *p == '\r')
        {
            p += 1;
            continue;
        }

        // comment
        if (!strncmp(p, "//", 2))
        {
            p += 2;

            while (*p != '\n')
                ++p;

            continue;
        }

        // if
        if (!strncmp(p, "if", 2) && !is_ident_body(p[2]))
        {
            cur = new_token(TK_IF, cur, p, 2);
            p += 2;
            continue;
        }

        // else
        if (!strncmp(p, "else", 4) && !is_ident_body(p[4]))
        {
            cur = new_token(TK_ELSE, cur, p, 4);
            p += 4;
            continue;
        }

        // while
        if (!strncmp(p, "while", 5) && !is_ident_body(p[5]))
        {
            cur = new_token(TK_WHILE, cur, p, 5);
            p += 5;
            continue;
        }

        // for
        if (!strncmp(p, "for", 3) && !is_ident_body(p[3]))
        {
            cur = new_token(TK_FOR, cur, p, 3);
            p += 3;
            continue;
        }

        // return
        if (!strncmp(p, "return", 6) && !is_ident_body(p[6]))
        {
            cur = new_token(TK_RETURN, cur, p, 6);
            p += 6;
            continue;
        }

        // switch
        if (!strncmp(p, "switch", 6) && !is_ident_body(p[6]))
        {
            cur = new_token(TK_SWITCH, cur, p, 6);
            p += 6;
            continue;
        }

        // case
        if (!strncmp(p, "case", 4) && !is_ident_body(p[4]))
        {
            cur = new_token(TK_CASE, cur, p, 4);
            p += 4;
            continue;
        }

        // break
        if (!strncmp(p, "break", 5) && !is_ident_body(p[5]))
        {
            cur = new_token(TK_BREAK, cur, p, 5);
            p += 5;
            continue;
        }

        // multi letter punctuator
        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">="))
        {
            cur = new_token(TK_PUNCT, cur, p, 2);
            p += 2;
            continue;
        }

        // single letter punctuator
        if (strchr("+-*/=()<>{};,", *p))
        {
            cur = new_token(TK_PUNCT, cur, p, 1);
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
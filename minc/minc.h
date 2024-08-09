#ifndef MINC_H
#define MINC_H

#include <stdbool.h>
#include <stdlib.h>


///////////////////////////////////////////////////////////////////////////
//
//    エラー報告
//

extern char *user_input;

void error(char *fmt, ...);

void error_at(char *loc, char *fmt, ...);

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
    size_t len;
};


extern Token *token;

// トークンを読み、期待した記号であるとき次へ進め真を返す
bool consume(char *op);

// トークンを読み、期待した記号であるとき次へ進め、それ以外の場合エラーを報告する
void expect(char *op);

// トークンを読み、数である場合それを返す。数でない場合エラーを報告する
int expect_number();

// p をトークナイズする
Token *tokenize(char *p);


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

    ND_EQ, // ==
    ND_NE, // !=
    ND_LT, // <
    ND_LE, // <=

    ND_NUM, // int
} NodeKind;

typedef struct Node Node;

struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *expr();


///////////////////////////////////////////////////////////////////////////
//
//    コードジェネレーター
//

void gen(Node *node);

#endif

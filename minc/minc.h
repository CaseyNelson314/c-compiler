#ifndef MINC_H
#define MINC_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

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
    TK_IDENT,    // 識別子
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

// トークンを読み、識別子であるとき次へ進め識別子のトークンを返す
Token *consume_ident();

// トークンを読み、期待した記号であるとき次へ進め、それ以外の場合エラーを報告する
void expect(char *op);

// トークンを読み、数である場合それを返す。数でない場合エラーを報告する
int expect_number();

// トークンを読み、末端である場合真を返す
bool at_eof();

// p をトークナイズする
Token *tokenize(char *p);

// ローカル変数
typedef struct LVar LVar;
struct LVar
{
    LVar *next;
    char *name;
    size_t len;
    int offset;
};

LVar* find_lvar(Token* token);

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

    ND_ASSIGN, // =

    ND_LVAR, // local variable

    ND_NUM, // integer
} NodeKind;

typedef struct Node Node;

struct Node
{
    NodeKind kind;

    Node *next; // プログラムに複数のステートメントが含まれる場合、このポインタを用いて線形リストを構成する

    Node *lhs; // AST right hand side
    Node *rhs; // AST left  hand side

    int val; // kind == ND_NUM

    int offset; // kind == ND_LVAR
};

Node *parse();

///////////////////////////////////////////////////////////////////////////
//
//    コードジェネレーター
//

void gen(Node *node);

#endif

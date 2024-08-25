///////////////////////////////////////////////////////////////////////////
//
//    パーサー
//

#include "minc.h"
#include <string.h>

static Node *new_node(NodeKind kind)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

static Node *new_node_binary(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

static Node *new_node_num(int val)
{
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

static LVar *local;
static LVar *find_lvar(Token *tok)
{
    for (LVar *var = local; var; var = var->next)
    {
        if (var->len == tok->len && strncmp(var->name, tok->str, var->len) == 0)
            return var;
    }
    return NULL;
}

static int new_lval(Token *tok)
{
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->name = tok->str;
    lvar->len = tok->len;

    if (local)
        lvar->offset = local->offset + 8; // 暗黙的に int 型を割り当て
    else
        lvar->offset = 8;

    // 先頭へ自身を追加
    lvar->next = local;
    local = lvar;

    return lvar->offset;
}

static Node *new_node_local()
{
    Node *node = new_node(ND_LVAR);

    Token *tok = expect_ident();

    LVar *lvar = find_lvar(tok);

    if (lvar)
        node->offset = lvar->offset;
    else
        node->offset = new_lval(tok);

    return node;
}

static Node *new_node_ident(Token *id_tok, TokenKind kind)
{
    Node *node = new_node(kind);
    node->id_name = id_tok->str;
    node->id_len = id_tok->len;
    return node;
}

static Node *stmt();
static Node *funcdef();
static Node *expr();
static Node *assign();
static Node *equality();
static Node *relational();
static Node *add();
static Node *mul();
static Node *unary();
static Node *primary();

// program = (function-definition)*
Node *parse()
{
    Node *head = calloc(1, sizeof(Node));
    head->kind = ND_ROOT;
    Node *cur = head;
    while (!at_eof())
    {
        cur = cur->next = funcdef();
    }
    return head;
}

// identlist = ident ("," ident)*
static Node *identlist()
{
    // 引数はローカル変数として定義されるものとする

    Node *head = new_node_local();
    Node *cur = head;

    while (skip(","))
    {
        cur = cur->next = new_node_local();
    }

    return head;
}

// function-definition = ident "(" identlist? ")" stmt
static Node *funcdef()
{
    Node *node = new_node_ident(expect_ident(), ND_FUNC_DEF);

    expect("(");

    if (!skip(")"))
    {
        node->func_idents = identlist();
        expect(")");
    }

    node->func_stmt = stmt();

    return node;
}

// stmt = expr? ";"
//      | "{" stmt* "}"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "while" "(" expr ")" stmt
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      | "return" expr ";"
static Node *stmt()
{
    // "{" stmt* "}"
    if (skip("{"))
    {
        // ステートメントの線形リストを構築
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;

        Node* cur = node;
        while (!skip("}"))
        {
            cur = cur->next = stmt();
        }

        return node;
    }

    // "if" "(" expr ")" stmt ("else" stmt)?
    if (consume_kind(TK_IF))
    {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_IF;

        expect("(");
        node->if_state = expr();
        expect(")");
        node->if_stmt = stmt();

        if (consume_kind(TK_ELSE))
        {
            node->else_stmt = stmt();
        }

        return node;
    }

    // "while" "(" expr ")" stmt
    if (consume_kind(TK_WHILE))
    {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;

        expect("(");
        node->while_state = expr();
        expect(")");
        node->while_stmt = stmt();

        return node;
    }

    // "for" "(" expr? ";" expr? ";" expr? ")" stmt
    if (consume_kind(TK_FOR))
    {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_FOR;

        expect("(");

        if (!skip(";")) // for ( の直後 ; である場合、初期化式がない
        {
            // 初期化式有り
            node->for_init = expr();
            expect(";");
        }

        if (!skip(";"))
        {
            // 条件式有り
            node->for_cond = expr();
            expect(";");
        }

        if (!skip(")"))
        {
            // ループ式有り
            node->for_loop = expr();
            expect(")");
        }

        node->for_stmt = stmt();

        return node;
    }

    // "return" expr ";"
    if (consume_kind(TK_RETURN))
    {
        Node *node = new_node(ND_RETURN);
        node->return_expr = expr();
        expect(";");
        return node;
    }

    // expr? ";"
    if (skip(";"))
    {
        return new_node(ND_NULL);
    }
    else
    {
        Node *node = expr();
        expect(";");
        return node;
    }
}

// expr = assign
static Node *expr()
{
    return assign();
}

// assign = equality ("=" assign)?
static Node *assign()
{
    Node *node = equality();
    if (skip("="))
        node = new_node_binary(ND_ASSIGN, node, assign());
    return node;
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality()
{
    Node *node = relational();
    for (;;)
    {
        if (skip("=="))
            node = new_node_binary(ND_EQ, node, relational());
        else if (skip("!="))
            node = new_node_binary(ND_NE, node, relational());
        else
            return node;
    }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
static Node *relational()
{
    Node *node = add();

    for (;;)
    {
        if (skip("<"))
            node = new_node_binary(ND_LT, node, add());
        else if (skip("<="))
            node = new_node_binary(ND_LE, node, add());
        else if (skip(">"))
            node = new_node_binary(ND_LT, add(), node);
        else if (skip(">="))
            node = new_node_binary(ND_LE, add(), node);
        else
            return node;
    }
}

// add = mul ("+" mul | "-" mul)*
static Node *add()
{
    Node *node = mul();

    for (;;)
    {
        if (skip("+"))
            node = new_node_binary(ND_ADD, node, mul());
        else if (skip("-"))
            node = new_node_binary(ND_SUB, node, mul());
        else
            return node;
    }
}

// mul = unary ("*" unary | "/" unary)*
static Node *mul()
{
    Node *node = unary();

    for (;;)
    {
        if (skip("*"))
            node = new_node_binary(ND_MUL, node, unary());
        else if (skip("/"))
            node = new_node_binary(ND_DIV, node, unary());
        else
            return node;
    }
}

// unary = ("+" | "-")? primary
static Node *unary()
{
    if (skip("+"))
        return primary();
    if (skip("-"))
        return new_node_binary(ND_SUB, new_node_num(0), primary());
    return primary();
}

// funcarg = expr ("," expr)*
struct Node *funcarg()
{
    Node *head = expr();

    Node *cur = head;

    while (skip(","))
    {
        cur = cur->next = expr();
    }

    return head;
}

// funccall = ident "(" funcarg? ")"
struct Node *funccall()
{
    Node *node = new_node_ident(expect_ident(), ND_FUNC_CALL);

    expect("(");

    if (skip(")"))
    {
        return node;
    }

    node->func_args = funcarg();
    expect(")");

    return node;
}

// primary = num
//         | ident
//         | ident ("(" function_args? ")")
//         | "(" expr ")"
static Node *primary()
{
    // "(" expr ")"
    if (skip("("))
    {
        Node *node = expr();
        expect(")");
        return node;
    }

    if (equal_kind(TK_IDENT))
    {
        if (equal(token->next, "("))
        {
            // 関数呼び出し
            return funccall();
        }

        // ローカル変数
        return new_node_local();
    }

    return new_node_num(expect_number());
}

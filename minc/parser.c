///////////////////////////////////////////////////////////////////////////
//
//    パーサー
//

#include "minc.h"
#include <string.h>

static Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

static Node *new_node_num(int val)
{
    Node *node = new_node(ND_NUM, NULL, NULL);
    node->val = val;
    return node;
}

static Node *stmt();
static Node *expr();
static Node *assign();
static Node *equality();
static Node *relational();
static Node *add();
static Node *mul();
static Node *unary();
static Node *primary();

// program = stmt*
Node *parse()
{
    Node head = {};
    Node *cur = &head;
    while (!at_eof())
    {
        cur = cur->next = stmt();
    }
    return head.next;
}

// stmt = expr ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "while" "(" expr ")" stmt
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      | "return" expr ";"
static Node *stmt()
{
    // "if" "(" expr ")" stmt ("else" stmt)?
    if (consume(TK_IF))
    {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_IF;

        expect("(");
        node->if_state = expr();
        expect(")");
        node->if_stmt = stmt();

        if (consume(TK_ELSE))
        {
            node->else_stmt = stmt();
        }

        return node;
    }

    // "while" "(" expr ")" stmt
    if (consume(TK_WHILE))
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

    // "return" expr ";"
    if (consume(TK_RETURN))
    {
        Node *node = new_node(ND_RETURN, expr(), NULL);
        expect(";");
        return node;
    }

    // expr ";"
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
    if (consume_punct("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

// equality = relational ("==" relational | "!=" relational)*
static Node *equality()
{
    Node *node = relational();
    for (;;)
    {
        if (consume_punct("=="))
            node = new_node(ND_EQ, node, relational());
        else if (consume_punct("!="))
            node = new_node(ND_NE, node, relational());
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
        if (consume_punct("<"))
            node = new_node(ND_LT, node, add());
        else if (consume_punct("<="))
            node = new_node(ND_LE, node, add());
        else if (consume_punct(">"))
            node = new_node(ND_LT, add(), node);
        else if (consume_punct(">="))
            node = new_node(ND_LE, add(), node);
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
        if (consume_punct("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume_punct("-"))
            node = new_node(ND_SUB, node, mul());
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
        if (consume_punct("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume_punct("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

// unary = ("+" | "-")? primary
static Node *unary()
{
    if (consume_punct("+"))
        return primary();
    if (consume_punct("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    return primary();
}

static LVar *local;
LVar *find_lvar(Token *token)
{
    for (LVar *var = local; var; var = var->next)
    {
        if (var->len == token->len && strncmp(var->name, token->str, var->len) == 0)
            return var;
    }
    return NULL;
}

LVar *new_lval(char *name)
{
}

// primary = num | ident | "(" expr ")"
static Node *primary()
{
    if (consume_punct("("))
    {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *token = consume(TK_IDENT);
    if (token)
    {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(token);
        if (lvar) // 既にローカル変数が定義されている場合
        {
            node->offset = lvar->offset;
        }
        else // 定義されていない場合、ローカル変数を線形リストへ登録
        {
            lvar = calloc(1, sizeof(LVar));
            lvar->name = token->str;
            lvar->len = token->len;
            lvar->next = local;
            if (local)
                lvar->offset = local->offset + 8; // 暗黙的に int 型を割り当て
            else
                lvar->offset = 0;
            node->offset = lvar->offset;
            local = lvar;
        }
        return node;
    }

    return new_node_num(expect_number());
}

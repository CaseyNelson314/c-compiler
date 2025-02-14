#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "minc.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        error("引数の個数が正しくありません");
    }

    user_input = open_file(argv[1]);

    token = tokenize(user_input);

    Node *node = parse();

    gen(node);

    return 0;
}

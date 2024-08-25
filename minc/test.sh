# !/bin/bash

# interp()
# {
#     input="$1"

#     ./main "$input" > tmp.s
#     cc -o tmp tmp.s function.c
#     ./tmp

#     echo "ret: $?"
# }

# interp "a = 1; a;"

test()
{
    filepath="$1"

    ./main "$filepath" > tmp.s
    cc -o tmp tmp.s ./test/lib.c
    ./tmp

    actual="$?"

    if [ "$actual" = 0 ]; then
        echo "[o] $filepath"
    else
        echo "[x] $filepath"
    fi
}

assert()
{
    # exit

    expected="$1"
    input="$2"

    ./main ">$input" > tmp.s
    cc -o tmp tmp.s ./test/lib.c
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

# テストファイルはリンクした関数を呼び出すため、関数呼び出し自体のテストを行う
assert 1 "main() { assert(0, 1); return 2; }"
assert 2 "main() { assert(1, 1); return 2; }"

test "./test/operator.c"
test "./test/compare.c"
test "./test/funccall.c"
test "./test/return.c"
test "./test/nullstmt.c"
test "./test/variable.c"
test "./test/flowctrl.c"

echo OK

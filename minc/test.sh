#!/bin/bash

# interp()
# {
#     input="$1"

#     ./main "$input" > tmp.s
#     cc -o tmp tmp.s function.c
#     ./tmp

#     echo "ret: $?"
# }

# interp "a = 100 / 2;"

assert()
{
    # exit

    expected="$1"
    input="$2"

    ./main "$input" > tmp.s
    cc -o tmp tmp.s function.c
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"
assert 2 "1 + 1;"
assert 2 "3 - 1;"
assert 2 "4 / 2;"
assert 21 "5 + 20 - 4;"
assert 12 "4 * (1 + 2);"
assert 10 "-10 + 20;"
assert 10 "-(-10);"
assert 10 "- (+ (-10));"

assert 1 "2 > 1;"
assert 0 "1 > 2;"
assert 0 "2 > 2;"
assert 0 "2 < 1;"
assert 0 "2 < 2;"
assert 1 "1 < 2;"
assert 1 "2 >= 1;"
assert 0 "1 >= 2;"
assert 1 "1 >= 1;"
assert 0 "2 <= 1;"
assert 1 "1 <= 2;"
assert 1 "1 <= 1;"
assert 1 "1 == 1;"
assert 0 "1 == 0;"
assert 1 "1 != 0;"
assert 0 "1 != 1;"

assert 0 "a = 0; a;"
assert 100 "a = b = 100;"
assert 120 "a = 100; b = 20; a + b;"
assert 1 "a = 1; aa = 2; a;"
assert 20 "aa = 10;bb = 20;cc = aa * bb;cc = cc / 10;cc;"

assert 10 "return 10;"
assert 10 "return 10; return 20;"
assert 10 "returna = 10; return returna;"

assert 1 "if (1) 1; else 2;"
assert 2 "if (0) 1; else 2;"
assert 1 "s = 1; if (s) 1;"
assert 2 "s = 0; if (s) 1; else 2;"

assert 5 "i = 0; while (i < 5) i = i + 1; i;"

assert 5 "for (n = 0; n < 5; n = n + 1) n; n;"

assert 6 "{ 2; 3; 4; { 5; 6; } }"

assert 123 "hogehoge();hogehoge();hogehoge();hogehoge();"

echo OK

#!/bin/bash

assert()
{
    expected="$1"
    input="$2"

    ./main "$input" > tmp.s
    cc -o tmp tmp.s
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
assert 21 "5 + 20 - 4;"
assert 12 "4 * (1 + 2);"
assert 10 "-10 + 20;"
assert 10 "-(-10);"
assert 10 "- (+ (-10));"

assert 0 "a = 0; a;"
assert 100 "a = b = 100;"
assert 120 "a = 100; b = 20; a + b;"
assert 1 "a = 1; aa = 2; a;"
assert 3 "aa = 10;bb = 20;cc = aa * bb;cc = cc / 10;cc;"

assert 1 "2 > 1;"
assert 1 "2 > 1;"
assert 0 "1 > 2;"
assert 0 "2 < 1;"
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

echo OK

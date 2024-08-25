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

assert 1 "main(){ return 1; }"
assert 1 "f(){ return 1; } main() { return f(); }"
assert 1 "main() { a = 1; return a; }"
assert 6 "mul(a, b) { return a * b; } main() { ret = mul(2, 3); return ret; }"

assert 1 "main() { ;; 1; }"
assert 2 "main() { { ;; };; { ;; } ;; 2; }"
assert 1 "main() { if (1) ; 1; }"

assert 0 "main() { 0; }"
assert 42 "main() { 42; }"
assert 2 "main() { 1 + 1; }"
assert 2 "main() { 3 - 1; }"
assert 2 "main() { 4 / 2; }"
assert 21 "main() { 5 + 20 - 4; }"
assert 12 "main() { 4 * (1 + 2); }"
assert 10 "main() { -10 + 20; }"
assert 10 "main() { -(-10); }"
assert 10 "main() { - (+ (-10)); }"

assert 1 "main() { 2 > 1; }"
assert 0 "main() { 1 > 2; }"
assert 0 "main() { 2 > 2; }"
assert 0 "main() { 2 < 1; }"
assert 0 "main() { 2 < 2; }"
assert 1 "main() { 1 < 2; }"
assert 1 "main() { 2 >= 1; }"
assert 0 "main() { 1 >= 2; }"
assert 1 "main() { 1 >= 1; }"
assert 0 "main() { 2 <= 1; }"
assert 1 "main() { 1 <= 2; }"
assert 1 "main() { 1 <= 1; }"
assert 1 "main() { 1 == 1; }"
assert 0 "main() { 1 == 0; }"
assert 1 "main() { 1 != 0; }"
assert 0 "main() { 1 != 1; }"

assert 0 "main() { a = 0; a; }"
assert 100 "main() { a = b = 100; }"
assert 120 "main() { a = 100; b = 20; a + b; }"
assert 1 "main() { a = 1; aa = 2; a; }"
assert 20 "main() { aa = 10;bb = 20;cc = aa * bb;cc = cc / 10;cc; }"

assert 10 "main() { return 10; }"
assert 10 "main() { return 10; return 20; }"
assert 10 "main() { returna = 10; return returna; }"

assert 1 "main() { if (1) 1; else 2; }"
assert 2 "main() { if (0) 1; else 2; }"
assert 1 "main() { s = 1; if (s) 1; }"
assert 2 "main() { s = 0; if (s) 1; else 2; }"

assert 5 "main() { i = 0; while (i < 5) i = i + 1; i; }"

assert 5 "main() { for (n = 0; n < 5; n = n + 1) n; n; }"

assert 6 "main() { { 2; 3; 4; { 5; 6; } } }"

assert 123 "main() { hogehoge();hogehoge();hogehoge();hogehoge(); }"

assert 6 "main() { sum(1,1,1,1,1,1); }"
assert 6 "main() { a=b=c=d=e=f=1; sum(a,b,c,d,e,f); }"
assert 11 "main() { sum(sum(1,1,1,1,1,1),1,1,1,1,1); }"
assert 16 "main() { sum(sum(sum(1,1,1,1,1,1),1,1,1,1,1),1,1,1,1,1); }"

echo OK

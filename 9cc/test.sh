#!/bin/bash

assert()
{
    expected="$1"
    input"$2"

    ./9cc "$input" > tmp.s
    clang -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$inpu => $expected expected, but got $actualt"
        exit 1
    fi
}

assert 0 0
assert 42 42

echo OK

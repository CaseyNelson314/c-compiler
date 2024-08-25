// null statement

f1() { ;; return 1; }
f2() { { ;; };; { ;; } ;; return 1; }
f3() { if (1) ; return 1; }
f4() { { 2; 3; 4; { 5; return 1; } } }

main()
{
    assert(f1(), 1);
    assert(f2(), 1);
    assert(f3(), 1);
    assert(f4(), 1);
    return 0;
}

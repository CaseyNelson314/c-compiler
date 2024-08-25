f0() { a = 0; return a; }
f1() { a = b = 100; return a; }
f2() { a = 100; b = 20; return a + b; }
f3() { a = 1; aa = 2; return a; }
f4() { aa = 10; bb = 20; cc = aa * bb; cc = cc / 10; return cc; }

main()
{
    assert(f0(), 0  );
    assert(f1(), 100);
    assert(f2(), 120);
    assert(f3(), 1  );
    assert(f4(), 20 );
    return 0;
}

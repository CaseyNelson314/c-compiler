f0() { if (1) return 1; else return 2; }
f1() { if (0) return 1; else return 2; }
f2() { s = 1; if (s) return 1; }
f3() { s = 0; if (s) return 1; else return 2; }

f4() { i = 0; while (i < 5) i = i + 1; return i; }

f5() { for (n = 0; n < 5; n = n + 1) n; n; }

main()
{
    assert(f0(), 1);
    assert(f1(), 2);
    assert(f2(), 1);
    assert(f3(), 2);
    assert(f4(), 5);
    assert(f5(), 5);
}
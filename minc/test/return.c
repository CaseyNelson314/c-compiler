f0() { return 10; }
f1() { return 10; return 20; }
f2() { returna = 10; return returna; }

main()
{
    assert(f0(), 10);
    assert(f1(), 10);
    assert(f2(), 10);
    return 0;
}
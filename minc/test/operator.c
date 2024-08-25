main()
{
    assert(0, 0);
    assert(42, 42);
    assert(1 + 1, 2);
    assert(3 - 1, 2);
    assert(4 / 2, 2);
    assert(5 + 20 - 4, 21);
    assert(4 * (1 + 2), 12);
    assert(-10 + 20, 10);
    assert(-(-10), 10);
    assert(-(+(-10)), 10);
    return 0;
}

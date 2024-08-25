assert(actual, expect)
{
    if (actual == expect)
        return 0;
    exit(1);
}

main()
{
    assert(2 > 1, 1);
    assert(1 > 2, 0);
    assert(2 > 2, 0);
    assert(2 < 1, 0);
    assert(2 < 2, 0);
    assert(1 < 2, 1);
    assert(2 >= 1, 1);
    assert(1 >= 2, 0);
    assert(1 >= 1, 1);
    assert(2 <= 1, 0);
    assert(1 <= 2, 1);
    assert(1 <= 1, 1);
    assert(1 == 1, 1);
    assert(1 == 0, 0);
    assert(1 != 0, 1);
    assert(1 != 1, 0);

    return 0;
}

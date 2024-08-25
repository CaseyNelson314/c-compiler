f0() { return 0; }
f1() { return 1; }

main()
{
    assert(f0(), 0);
    assert(f1(), 1);
    
    assert(hoge(), 123);
    assert(sum(1,1,1,1,1,1), 6);

    a=b=c=d=e=f=1;
    assert(sum(a,b,c,d,e,f), 6);

    assert(sum(sum(1,1,1,1,1,1),1,1,1,1,1), 11);
    assert(sum(sum(sum(1,1,1,1,1,1),1,1,1,1,1),1,1,1,1,1), 16);
}

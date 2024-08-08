# a simple c compiler

<https://www.sigbus.info/compilerbook>

## build

```sh
git clone https://github.com/CaseyNelson314/c-compiler.git
cd c-compiler/minc
make
```

## run

```sh
./main "1 + 20 - 10" > tmp.s
cc -o tmp tmp.s
./tmp
echo $?
```

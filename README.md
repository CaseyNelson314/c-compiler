# a simple c compiler

<https://www.sigbus.info/compilerbook>

## 遊び方

build

```sh
git clone https://github.com/CaseyNelson314/c-compiler.git
cd c-compiler/minc
make
```

unit test

```sh
make test
```

## 処理順

1. 字句解析 [<tokenizer.c>](./minc/tokenizer.c)

   文字列をトークン列に分割

   > 長いトークンから解析する

2. 構文解析 [<parser.c>](./minc/parser.c)

   BNF 記法で表された C 言語の文法規則を基に、構文木を作成

   > BNF の定義次第でパーサーの書きやすさが変わる
   >
   > <https://cs.wmich.edu/~gupta/teaching/cs4850/sumII06/The%20syntax%20of%20C%20in%20Backus-Naur%20form.htm>

3. コード生成 [<condegen.c>](./minc/codegen.c)

   構文木を末端から評価していき、アセンブリコードを生成

   > 評価値を rax レジスタへ乗せると楽

## テストについて

テストコードをコンパイルさせ、実行し、目的の値が得られるかを検証する。

テストコードは自作コンパイラによってコンパイルされるため、常に 0 を返す(成功)様なコンパイラを作れてしまうことを考えると、テストの信頼性がなくなる。

そこでテストで使用している機能(テスト用関数呼び出し)の動作検証をシェルスクリプト上で検証することで信頼性を上げている。

テスト用関数は別ファイルに定義し既存のコンパイルでコンパイルすることで、シェルスクリプト上での検証量を減らしている。

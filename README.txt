(Japanese, UTF-8)

# renum by katahiromz

## これは何？

`renum` は、BASIC プログラムの行番号を再番号付けするためのコマンドラインツールです。
このツールを使用すると、BASIC コードの行番号を体系的に更新でき、コードが読みやすく、管理しやすくなります。

```txt
RENUM --- BASIC プログラムの行番号を再番号付け

使用方法: renum [OPTIONS] -i your_file.bas -o output.bas

オプション:
  -i FILE                  再番号付けする BASIC 入力ファイルを指定します。
  -o FILE                  出力ファイルを指定します (デフォルト: output.bas)。
  --new-start LINE_NUMBER  新しい開始行番号を設定します (デフォルト: 10)。
  --old-start LINE_NUMBER  古い開始行番号を設定します (デフォルト: 0)。
  --step STEP              行番号の増加ステップを設定します (デフォルト: 10)。
  --force                  無効な行番号があっても強制的に再番号付けを行います。
  --help                   このヘルプメッセージを表示して終了します。
  --version                バージョン情報を表示して終了します。
```

## 使用例

```cmd
renum -i program.bas -o new_program.bas --new-start 100 --step 20
```

## 主な機能

- 柔軟な再番号付け: 開始行番号や増加ステップをカスタマイズしてニーズに合わせることができます。
- エラー処理: 無効な行番号に遭遇しても、オプションで再番号付けを強制できます。
- ファイルサポート: 入力ファイルと出力ファイルを簡単に指定して効率的な作業が可能です。

## サポートする文

(### は行番号を表します)

- `GOTO ###`
- `GOSUB ###`
- `GO TO ###`
- `GO SUB ###`
- `IF ...式... THEN ### ELSE ###`
- `ON ...式... GOTO ###, ###, ...`
- `ON ...式... GOSUB ###, ###, ...`
- `AUTO ###`
- `EDIT ###`
- `DELETE ###`
- `DELETE ###-`
- `DELETE -###`
- `DELETE ###-###`
- `LIST ###`
- `LIST ###-`
- `LIST ###-###`
- `LIST -###`
- `LLIST ###`
- `LLIST ###-`
- `LLIST ###-###`
- `LLIST -###`
- `RESUME ###`
- `RUN ###`
- `RESTORE ###`
- `RETURN ###`

## ライセンス

- MIT

## お問い合わせ

このツールの詳細や使用に関するサポートについては katahiromz にお問い合わせください。

- 片山博文MZ <katayama.hirofumi.mz@gmail.com>

---
(English)

# renum by katahiromz

## What's this?

The `renum` program is a command-line tool designed to renumber the lines in a BASIC program.
It helps organize and manage your BASIC code by systematically updating line numbers,
making the code easier to read and maintain.

```txt
RENUM --- Renumber BASIC Program Lines

Usage: renum [OPTIONS] -i your_file.bas -o output.bas

Options:
  -i FILE                  Specify the input BASIC file to be renumbered.
  -o FILE                  Specify the output file (default: output.bas).
  --new-start LINE_NUMBER  Set the new starting line number (default: 10).
  --old-start LINE_NUMBER  Set the old starting line number (default: 0).
  --step STEP              Set the increment step between lines (default: 10).
  --force                  Force renumbering even if any invalid line number.
  --help                   Display this help message and exit.
  --version                Display version information and exit.
```

## Example

```cmd
renum -i program.bas -o new_program.bas --new-start 100 --step 20
```

## Key Features

- *Flexible Renumbering*: Customize the starting line number and step increment to fit your needs.
- *Error Handling*: Options to handle or force renumbering even when encountering invalid line numbers.
- *File Support*: Easily specify input and output files for streamlined workflow.

## Covering Statements

(`###` represents a line number)

- `GOTO ###`
- `GOSUB ###`
- `GO TO ###`
- `GO SUB ###`
- `IF ...expr... THEN ### ELSE ###`
- `ON ...expr... GOTO ###, ###, ...`
- `ON ...expr... GOSUB ###, ###, ...`
- `AUTO ###`
- `EDIT ###`
- `DELETE ###`
- `DELETE ###-`
- `DELETE -###`
- `DELETE ###-###`
- `LIST ###`
- `LIST ###-`
- `LIST ###-###`
- `LIST -###`
- `LLIST ###`
- `LLIST ###-`
- `LLIST ###-###`
- `LLIST -###`
- `RESUME ###`
- `RUN ###`
- `RESTORE ###`
- `RETURN ###`

## License

- MIT

## Contact

For more details or help using this tool, you can contact katahiromz.

- Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>

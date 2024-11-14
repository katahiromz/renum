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

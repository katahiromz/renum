# renum by katahiromz

## What's this?

This `renum` program is a command-line tool to renumber a BASIC program lines.

```txt
RENUM --- Renumber BASIC Program Lines

Usage: renum [OPTIONS] -i your_file.bas -o output.bas

Options:
  -i FILE              Specify the input BASIC file to be renumbered.
  -o FILE              Specify the output file (default: output.bas).
  --start LINE_NUMBER  Set the starting line number (default: 10).
  --step STEP          Set the increment step between lines (default: 10).
  --force              Force renumbering even if any invalid line number.
  --help               Display this help message and exit.
  --version            Display version information and exit.
```

## Example

```cmd
renum -i program.bas -o new_program.bas --start 100 --step 20
```

## Covering Statements

(`###` represents a line number)

- `GOTO ###`
- `GOSUB ###`
- `GO TO ###`
- `GO SUB ###`
- `IF ...expr... THEN ### ELSE ###`
- `ON ...expr... GOTO ###, ###, ...`
- `ON ...expr... GOSUB ###, ###, ...`
- `EDIT ###`
- `DELETE ###`
- `DELETE ###-`
- `DELETE -###`
- `DELETE ###-###`
- `RESUME ###`
- `RUN ###`
- `RESTORE ###`
- `RETURN ###`
- `AUTO ###`

## License

- MIT

## Contact

- Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>

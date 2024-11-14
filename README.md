# renum by katahiromz

This `renum` program is a command-line tool to renumber a BASIC program lines.

```txt
RENUM --- Renumber BASIC program lines

Usage: renum [OPTIONS] -i your_file.bas -o output.bas

Options:
  -i FILE                  The input file
  -o FILE                  The output file (default: output.bas)
  --start LINE_NUMBER      The starting line number (default: 10)
  --step  LINE_NUMBER      The step between line numbers (default: 10)
  --force                  Force even if there is an invalid line number
  --help                   Display this message
  --version                Display version information

Contact: katayama.hirofumi.mz@gmail.com
```

License: MIT

# nonascii
Scans text files and indicates non-7-bit ASCII characters.


## Introduction

There are some tools that don't like text files with non-standard ASCII
characters in it (UTF-8, ISO-8859, etc).
This tool is used to identify files with such characters, and the
locations of those characters.


## Usage

````
Usage: nonascii [-h] [-n num_reports] [--] [in_file ...]
where:
  -h - print help
  -- - indicates end of options.
  -n num_reports - only print up to 'num_reports' lines of output
    for each input file. 0 means no limit (infinite).
  in_file - zero or more text files. If none, use stdin. A file named
    '-' indicates to read standard input.
For details, see https://github.com/fordsfords/nonascii
````

Here are a few examples:

````
nonascii -- -a b - c <d
````

The "--" option tells nonascii that there are no more options.
Thus, "-a" is treated as the first input file.
"b" is the second input file.
"-" is the third, but "-" is tells nonascii to read from standard input,
which is file "d". Thus, the third file read is "d".
"c" is the fourth file read.

But what if you have a file that is named "-"?

````
nonascii -- - <-
````

Again, the "--" option tells nonascii to stop looking for options.
Thus, the first file read is "-", which tells non ascii to read
standard input.
Standard input is set to the file "-".


## License

I want there to be NO barriers to using this code, so I am releasing it to the public domain.  But "public domain" does not have an internationally agreed upon definition, so I use CC0:

This work is dedicated to the public domain under CC0 1.0 Universal:
http://creativecommons.org/publicdomain/zero/1.0/

To the extent possible under law, Steven Ford has waived all copyright
and related or neighboring rights to this work. In other words, you can 
use this code for any purpose without any restrictions.
This work is published from: United States.
Project home: https://github.com/fordsfords/nonascii

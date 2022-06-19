# chars
Split files into characters, with additional formatting and escaping options. 


Example
-------

    $ cat test-in.txt
    Hello, World!
    This is line two.

    $ chars --oneline
    H e l l o , \40 W o r l d ! \n T h i s \40 i s \40 l i n e \40 t w o . \n

    $ chars --oneline --only printable
    H e l l o , W o r l d ! T h i s i s l i n e t w o .

    $ chars --oneline -Hex
    48 65 6C 6C 6F 2C 20 57 6F 72 6C 64 21 0A 54 68 69 73 20 69 73 20 6C 69 6E
    65 20 74 77 6F 2E 0A

Usage
-----

`chars -h`

    Usage: chars [OPTIONS]

    Split files into characters, one per line or separated by spaces, with
    additional formatting and escaping options.

    General options:
     -v, --version          print version number and exit
     -h, --help             print this message and exit
     -i, --input   <file>   path or - for standard input (default)
     -o, --output  <file>   path or - for standard output (default)
         --only    <class>  print 'all' or 'printable' (default) characters
     -p                     shortcut for  --only printable
         --multiline        print one character per line (default)
         --oneline          print the characters separated by spaces
         --[no-]escape      escape non-printable characters in C style
                            (only for --chars; default: enabled)
     -q, --[no-]quote       quote characters with " (default: no)

    Output style options with printf equivalents:
     -c, --chars            ASCII characters (default)         %c
     -u, --decimal          ASCII codepoints                   %3i
     -x, --hex              ASCII codepoints in hex            %02x
     -X, --Hex              ASCII codepoints in uppercase hex  %02X

    Also see the man page: man chars

<!--
Building & Installing
---------------------

    make
    make check    # optional; runs tests
    make install  # change prefix with   prefix=/some/path

    # to regenerate the test cases when needed
    make test-out
-->

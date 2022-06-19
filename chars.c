#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#define PROG_NAME "chars"
#define PROG_VER "0.1"

static const char* const usage =
"Usage: " PROG_NAME " [OPTIONS]\n\n"
"Split files into characters, one per line or separated by spaces, with\n"
"additional formatting and escaping options.\n\n"
"General options:\n"
" -v, --version          print version number and exit\n"
" -h, --help             print this message and exit\n"
" -i, --input   <file>   path or - for standard input (default)\n"
" -o, --output  <file>   path or - for standard output (default)\n"
"     --only    <class>  print 'all' or 'printable' (default) characters\n"
" -p                     shortcut for  --only printable\n"
"     --multiline        print one character per line (default)\n"
"     --oneline          print the characters separated by spaces\n"
"     --[no-]escape      escape non-printable characters in C style\n"
"                        (only for --chars; default: enabled)\n"
" -q, --[no-]quote       quote characters with \" (default: no)\n\n"
"Output style options with printf equivalents:\n"
" -c, --chars            ASCII characters (default)         %%c\n"
" -u, --decimal          ASCII codepoints                   %%3i\n"
" -x, --hex              ASCII codepoints in hex            %%02x\n"
" -X, --Hex              ASCII codepoints in uppercase hex  %%02X\n\n"
"Also see the man page: man chars\n";

struct fmt_entry {
    const char* short_opt;
    const char* long_opt;
    const char* fmt;
};

static const struct fmt_entry fmt_table[] = {
    { "-c", "--chars", "%c" },
    { "-u", "--decimal", "%3u" },
    { "-x", "--hex", "%02x" },
    { "-X", "--Hex", "%02X" }
};

static void missing_val(const char* optname)
{
    fprintf(stderr, PROG_NAME ": missing argument for %s\n", optname);
    exit(EXIT_FAILURE);
}

static const char* path_from_opt(const char* val)
{
    if (strcmp(val, "-") == 0)
        return NULL;
    else
        return val;
}

static const char* fmt_from_opt(const char* opt)
{
    int table_len = sizeof(fmt_table) / sizeof(fmt_table[0]);
    for (int i = 0; i < table_len; i++)
    {
        const char* s = fmt_table[i].short_opt;
        const char* l = fmt_table[i].long_opt;
        if ((s && strcmp(opt, s) == 0) || (l && strcmp(l, opt) == 0))
            return fmt_table[i].fmt;

    }

    return NULL;
}

static FILE* open_or_default(const char* path, const char* mode,FILE* fallback)
{
    if (!path)
        return fallback;

    FILE* file = fopen(path, mode);
    if (!file) {
        perror(PROG_NAME);
        exit(EXIT_FAILURE);
    }

    return file;
}

static const char* escape_char(int ch)
{
    static char buf[5];

    switch (ch) {
        case '\a': return "\\a";
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        case '\v': return "\\v";
        case '\'': return "\\'";
        case '\"': return "\\\"";
        case '\\': return "\\\\";

                   snprintf(buf, sizeof(buf), "\\%o", ch & 0xFF);
            default:
                   return buf;
    }
}

int main(int argc,const char *argv[])
{
    const char* fmt = "%c";
    const char* inpath = NULL;
    const char* outpath = NULL;
    bool only_printable = false;
    bool oneline = false;
    bool escape = true;
    bool quote = false;

    for(int i=1;i<argc;i++)
    {
        const char* arg = argv[i];
        const char* new_fmt;
        if(strcmp("-v",arg)==0 || strcmp("--version",arg)==0 )
        {
            printf(PROG_VER,"\n");
            exit(EXIT_SUCCESS);
        }
        else if (strcmp("-h", arg) == 0 || strcmp("--help", arg) == 0)
        {
            printf(usage);
            exit(EXIT_SUCCESS);
        }
        else if(strcmp("-i",arg)==0 || strcmp("--input",arg)==0 )
        {
            if(argc<=i+1)
                missing_val(arg);
            inpath=path_from_opt(argv[++i]);
        }
        else if(strcmp("-o",arg)==0 || strcmp("--outpath",arg)==0 )
        {
            if (argc <= i + 1)
                missing_val(arg);
            outpath = path_from_opt(argv[++i]);
        }
        else if(strcmp("--only",arg)==0)
        {
            if (argc <= i + 1)
                missing_val(arg);
            const char* val = argv[++i];
            if (strcmp("all", val) == 0)
                only_printable = false;
            else if (strcmp("printable", val) == 0)
                only_printable = true;
            else
            {
                fprintf(stderr, PROG_NAME ": "
                        "invalid value for %s, try -h\n",
                        arg);
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp("-p",arg)==0)
            only_printable=true;
        else if(strcmp("--multiline",arg)==0)
            oneline=false;
        else if (strcmp("--oneline", arg) == 0)
            oneline = true;
        else if (strcmp("--escape", arg) == 0)
            escape = true;
        else if (strcmp("--no-escape", arg) == 0)
            escape = false;
        else if (strcmp("-q", arg) == 0 || strcmp("--quote", arg) == 0)
            quote = true;
        else if (strcmp("--no-quote", arg) == 0)
            quote = false;
        else if ((new_fmt = fmt_from_opt(arg)))
            fmt = new_fmt;
        else if (arg[0] == '-')
        {
            fprintf(stderr, PROG_NAME ": "
                    "unrecognized option: %s, try -h\n", arg);
            exit(EXIT_FAILURE);
        }
        else
        {
            fprintf(stderr, PROG_NAME ": "
                    "unexpected argument: %s, try -h\n", arg);
            exit(EXIT_FAILURE);
        }
    }

    FILE* infile = open_or_default(inpath, "r", stdin);
    FILE* outfile = open_or_default(outpath, "w", stdout);

    int ch;
    bool first=true;

    while((ch=fgetc(infile)) !=EOF)
    {
        if(only_printable && !isprint(ch))
            continue;

        if(first)
            first = false;
        else if (oneline)
            fputc(' ', outfile);
        if(quote)
            fputc('"', outfile);

        if(escape && fmt[1]=='c' && (!isprint(ch) || (ch == ' ' && oneline && !quote)))
            fputs(escape_char(ch),outfile);
        else
            fprintf(outfile, fmt, ch);

        if(quote)
            fputc('"', outfile);
        if(!oneline)
            fputc('\n', outfile);
    }

    if(oneline)
        fputc('\n', outfile);

    if(ferror(infile))
    {
        perror(PROG_NAME);
        exit(EXIT_FAILURE);
    }
}

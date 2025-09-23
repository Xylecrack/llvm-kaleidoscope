#include "lexer.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>

std::string IdentifierStr;
double NumVal;

int gettok()
{
    static int lastchar = ' ';
    // skip any whitespacce
    while (isspace(lastchar))
    {
        lastchar = getchar();
    }

    if (isalpha(lastchar))
    {
        IdentifierStr = lastchar;
        while (isalnum(lastchar = getchar()))
        {
            IdentifierStr += lastchar;
        }
        if (IdentifierStr == "def")
        {
            return tok_def;
        }
        if (IdentifierStr == "extern")
        {
            return tok_extern;
        }
        return tok_identifier;
    }

    // TODO: handle invalid nums (ex: strod reads 1.2.3 as 1.2)
    if (isdigit(lastchar) || lastchar == '.')
    {
        std::string numstr;
        do
        {
            numstr += lastchar;
            lastchar = getchar();

        } while (isdigit(lastchar) || lastchar == '.');
        NumVal = strtod(numstr.c_str(), 0);
        return tok_number;
    }

    if (lastchar == '#')
    {
        do
        {
            lastchar = getchar();

        } while (lastchar != EOF && lastchar != '\n' && lastchar != '\r');
        if (lastchar != EOF)
        {
            return gettok();
        }
    }
    if (lastchar == EOF)
    {
        return tok_eof;
    }
    int thischar = lastchar;
    lastchar = getchar();
    return thischar;
}

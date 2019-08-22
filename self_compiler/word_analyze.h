#ifndef WORD_ANALYZE
#define WORD_ANALYZE
#include<stdio.h>
#include <iostream>

int insymbol();

extern enum symbol{ zerocon, unsignintcon, charcon, stringcon,
plu, minu, times, divs,
eql, neq, gtr, geq, lss, leq,
lparent, rparent, lbrack, rbrack, llf, rrf, comma, semicolon, colon, becomes, 
ident,
ifsy, dosy, whilesy, switchsy, casesy,
scanfsy, printfsy, returnsy,eofsy,
constsy, intsy, charsy, voidsy,
wrongsy};

extern int words_sum;
extern char ch;
extern int cc,ll,lines;
extern char line[];
extern char name[];
extern int sum;
extern symbol sy;

extern FILE * in;
extern FILE * out;
extern FILE * papage;

#endif
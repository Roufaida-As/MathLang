%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "expr_info.h"
#include "mathlang.tab.h"

int line_num = 1;
int col_num  = 1;

#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = line_num; \
    yylloc.first_column = col_num; \
    yylloc.last_column = col_num + yyleng - 1; \
    col_num += yyleng;

void handle_newline() { 
    line_num++; 
    col_num = 1; 
}
%}

/* Options */
%option noyywrap
%option noinput
%option nounput

/* Macros */
DIGIT       [0-9]
ID_START    [a-zA-Z_]
ID_CHAR     [a-zA-Z0-9_]
WHITESPACE  [ \t\r]
NEWLINE     \n

%%

"SOIT"              { return TOK_SOIT; }
"SOIT_CONST"        { return TOK_SOIT_CONST; }
"tel"[ \t]+"que"    { return TOK_TEL_QUE; }
"TYPE"              { return TOK_TYPE; }
"ENREGISTREMENT"    { return TOK_ENREGISTREMENT; }

"SI"        { return TOK_SI; }
"ALORS"     { return TOK_ALORS; }
"SINON"     { return TOK_SINON; }
"FIN"       { return TOK_FIN; }

"TANT"      { return TOK_TANT; }
"QUE"       { return TOK_QUE; }
"FAIRE"     { return TOK_FAIRE; }

"POUR"      { return TOK_POUR; }
"DE"        { return TOK_DE; }
"A"         { return TOK_A; }
"PAR"       { return TOK_PAR; }

"REPETER"   { return TOK_REPETER; }
"JUSQUA"    { return TOK_JUSQUA; }

"SORTIR"    { return TOK_SORTIR; }
"CONTINUER" { return TOK_CONTINUER; }

"AFFICHER"        { return TOK_AFFICHER; }
"AFFICHER_LIGNE"  { return TOK_AFFICHER_LIGNE; }
"LIRE"            { return TOK_LIRE; }

"FONCTION"   { return TOK_FONCTION; }
"PROCEDURE"  { return TOK_PROCEDURE; }
"RETOURNER"  { return TOK_RETOURNER; }
"LAMBDA"     { return TOK_LAMBDA; }

"vrai"   { return TOK_TRUE; }
"faux"   { return TOK_FALSE; }

"et"   { return TOK_AND; }
"ou"   { return TOK_OR; }
"non"  { return TOK_NOT; }
"xor"  { return TOK_XOR; }

"Z"      { return TOK_TYPE_Z; }
"R"      { return TOK_TYPE_R; }
"B"      { return TOK_TYPE_B; }
"C"      { return TOK_TYPE_C; }
"Sigma"  { return TOK_TYPE_SIGMA; }
"Char"   { return TOK_TYPE_CHAR; }

"int8"   { return TOK_INT8; }
"int16"  { return TOK_INT16; }
"int32"  { return TOK_INT32; }
"int64"  { return TOK_INT64; }
"float"  { return TOK_FLOAT_TYPE; }
"double" { return TOK_DOUBLE_TYPE; }

"sin"   { return TOK_SIN; }
"cos"   { return TOK_COS; }
"exp"   { return TOK_EXP; }
"log"   { return TOK_LOG; }
"sqrt"  { return TOK_SQRT; }
"abs"   { return TOK_ABS; }
"floor" { return TOK_FLOOR; }
"ceil"  { return TOK_CEIL; }
"round" { return TOK_ROUND; }

"int"      { return TOK_INT_FUNC; }
"reel"     { return TOK_REEL; }
"booleen"  { return TOK_BOOLEEN; }
"chaine"   { return TOK_CHAINE; }

"re"   { return TOK_RE; }
"im"   { return TOK_IM; }
"arg"  { return TOK_ARG; }

"majuscules"  { return TOK_MAJUSCULES; }
"minuscules"  { return TOK_MINUSCULES; }
"diviser"     { return TOK_DIVISER; }

"det"        { return TOK_DET; }
"inv"        { return TOK_INV; }
"trace"      { return TOK_TRACE; }
"identite"   { return TOK_IDENTITE; }

"dans"        { return TOK_IN; }
"/∈"|"notin"  { return TOK_NOT_IN; }
"union"       { return TOK_UNION; }
"inter"       { return TOK_INTER; }
"diff"|"\\\\" { return TOK_DIFF; }
"symdiff"     { return TOK_SYMDIFF; }
"inclus"      { return TOK_SUBSET; }
"strict_inclus" { return TOK_STRICT_SUBSET; }

"<=>" { return TOK_EQUIV; }
"<-"  { return TOK_ASSIGN; }
"->"  { return TOK_ARROW; }
"=>"  { return TOK_IMPLIES; }
"!="  { return TOK_NEQ; }
"<="  { return TOK_LEQ; }
">="  { return TOK_GEQ; }
"="   { return TOK_EQ; }
"<"   { return TOK_LT; }
">"   { return TOK_GT; }

"+"   { return TOK_PLUS; }
"-"   { return TOK_MINUS; }
"*"   { return TOK_MULT; }
"/"   { return TOK_DIV_REAL; }
"div" { return TOK_DIV; }
"mod" { return TOK_MOD; }
"^"   { return TOK_POWER; }

"(" { return TOK_LPAREN; }
")" { return TOK_RPAREN; }
"[" { return TOK_LBRACKET; }
"]" { return TOK_RBRACKET; }
"{" { return TOK_LBRACE; }
"}" { return TOK_RBRACE; }
"," { return TOK_COMMA; }
":" { return TOK_COLON; }
"." { return TOK_DOT; }
"|" { return TOK_PIPE; }
";" { return TOK_SEMICOLON; }

"#".*  { /* comment - do nothing */ }

{DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?i |
{DIGIT}+[eE][+-]?{DIGIT}+i |
{DIGIT}+\.([eE][+-]?{DIGIT}+)?i |
\.{DIGIT}+([eE][+-]?{DIGIT}+)?i |
{DIGIT}+i {
    yylval.strval = strdup(yytext);
    return TOK_COMPLEX;
}

{DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)? |
{DIGIT}+[eE][+-]?{DIGIT}+ |
{DIGIT}+\. |
\.{DIGIT}+ {
    yylval.floatval = atof(yytext);
    return TOK_FLOAT;
}

{DIGIT}+ {
    yylval.intval = atoi(yytext);
    return TOK_INT;
}

\"([^\"\\]|\\.)*\" {
    yylval.strval = strdup(yytext);
    return TOK_STRING;
}

\'([^\'\\]|\\.)\' {
    if (yytext[1] == '\\') {
        switch (yytext[2]) {
            case 'n':  yylval.charval = '\n'; break;
            case 't':  yylval.charval = '\t'; break;
            case 'r':  yylval.charval = '\r'; break;
            case '\\': yylval.charval = '\\'; break;
            case '\'': yylval.charval = '\''; break;
            case '0':  yylval.charval = '\0'; break;
            default:   yylval.charval = yytext[2];
        }
    } else {
        yylval.charval = yytext[1];
    }
    return TOK_CHAR;
}

{ID_START}{ID_CHAR}* {
    yylval.strval = strdup(yytext);
    return TOK_ID;
}

{WHITESPACE}+  { /* skip whitespace */ }
{NEWLINE}      { handle_newline(); }

. {
    fprintf(stderr, "Erreur lexicale à la ligne %d, colonne %d: caractère inconnu '%s'\n",
            line_num, col_num, yytext);
}

%%
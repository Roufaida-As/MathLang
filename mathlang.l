%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "mathlang.tab.h"

int line_num = 1;
int col_num  = 1;

void update_position() { col_num += yyleng; }
void handle_newline()  { line_num++; col_num = 1; }
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

"SOIT"              { update_position(); return TOK_SOIT; }
"SOIT_CONST"        { update_position(); return TOK_SOIT_CONST; }
"tel"[ \t]+"que"    { update_position(); return TOK_TEL_QUE; }
"TYPE"              { update_position(); return TOK_TYPE; }
"ENREGISTREMENT"    { update_position(); return TOK_ENREGISTREMENT; }

"SI"        { update_position(); return TOK_SI; }
"ALORS"     { update_position(); return TOK_ALORS; }
"SINON"     { update_position(); return TOK_SINON; }
"FIN"       { update_position(); return TOK_FIN; }

"TANT"      { update_position(); return TOK_TANT; }
"QUE"       { update_position(); return TOK_QUE; }
"FAIRE"     { update_position(); return TOK_FAIRE; }

"POUR"      { update_position(); return TOK_POUR; }
"DE"        { update_position(); return TOK_DE; }
"A"         { update_position(); return TOK_A; }
"PAR"       { update_position(); return TOK_PAR; }

"REPETER"   { update_position(); return TOK_REPETER; }
"JUSQUA" { update_position(); return TOK_JUSQUA; }

"SORTIR"    { update_position(); return TOK_SORTIR; }
"CONTINUER" { update_position(); return TOK_CONTINUER; }

"AFFICHER"        { update_position(); return TOK_AFFICHER; }
"AFFICHER_LIGNE"  { update_position(); return TOK_AFFICHER_LIGNE; }
"LIRE"            { update_position(); return TOK_LIRE; }

"FONCTION"   { update_position(); return TOK_FONCTION; }
"PROCEDURE"  { update_position(); return TOK_PROCEDURE; }
"RETOURNER"  { update_position(); return TOK_RETOURNER; }
"LAMBDA"     { update_position(); return TOK_LAMBDA; }

"vrai"   { update_position(); return TOK_TRUE; }
"faux"   { update_position(); return TOK_FALSE; }

"et"   { update_position(); return TOK_AND; }
"ou"   { update_position(); return TOK_OR; }
"non"  { update_position(); return TOK_NOT; }
"xor"  { update_position(); return TOK_XOR; }

"Z"      { update_position(); return TOK_TYPE_Z; }
"R"      { update_position(); return TOK_TYPE_R; }
"B"      { update_position(); return TOK_TYPE_B; }
"C"      { update_position(); return TOK_TYPE_C; }
"Sigma"  { update_position(); return TOK_TYPE_SIGMA; }
"Char"   { update_position(); return TOK_TYPE_CHAR; }

"int8"   { update_position(); return TOK_INT8; }
"int16"  { update_position(); return TOK_INT16; }
"int32"  { update_position(); return TOK_INT32; }
"int64"  { update_position(); return TOK_INT64; }
"float"  { update_position(); return TOK_FLOAT_TYPE; }
"double" { update_position(); return TOK_DOUBLE_TYPE; }

"sin"   { update_position(); return TOK_SIN; }
"cos"   { update_position(); return TOK_COS; }
"exp"   { update_position(); return TOK_EXP; }
"log"   { update_position(); return TOK_LOG; }
"sqrt"  { update_position(); return TOK_SQRT; }
"abs"   { update_position(); return TOK_ABS; }
"floor" { update_position(); return TOK_FLOOR; }
"ceil"  { update_position(); return TOK_CEIL; }
"round" { update_position(); return TOK_ROUND; }

"int"      { update_position(); return TOK_INT_FUNC; }
"reel"     { update_position(); return TOK_REEL; }
"booleen"  { update_position(); return TOK_BOOLEEN; }
"chaine"   { update_position(); return TOK_CHAINE; }

"re"   { update_position(); return TOK_RE; }
"im"   { update_position(); return TOK_IM; }
"arg"  { update_position(); return TOK_ARG; }

"majuscules"   { update_position(); return TOK_MAJUSCULES; }
"minuscules"  { update_position(); return TOK_MINUSCULES; }
"diviser"     { update_position(); return TOK_DIVISER; }

"det"        { update_position(); return TOK_DET; }
"inv"        { update_position(); return TOK_INV; }
"trace"      { update_position(); return TOK_TRACE; }
"identite"   { update_position(); return TOK_IDENTITE; }

"dans"        { update_position(); return TOK_IN; }
"/∈"|"notin"  { update_position(); return TOK_NOT_IN; }
"union"       { update_position(); return TOK_UNION; }
"inter"       { update_position(); return TOK_INTER; }
"diff"|"\\\\" { update_position(); return TOK_DIFF; }
"symdiff"     { update_position(); return TOK_SYMDIFF; }
"inclus"      { update_position(); return TOK_SUBSET; }
"strict_inclus" { update_position(); return TOK_STRICT_SUBSET; }

"<=>" { update_position(); return TOK_EQUIV; }
"<-"  { update_position(); return TOK_ASSIGN; }
"->"  { update_position(); return TOK_ARROW; }
"=>"  { update_position(); return TOK_IMPLIES; }
"!="  { update_position(); return TOK_NEQ; }
"<="  { update_position(); return TOK_LEQ; }
">="  { update_position(); return TOK_GEQ; }
"="   { update_position(); return TOK_EQ; }
"<"   { update_position(); return TOK_LT; }
">"   { update_position(); return TOK_GT; }

"+"   { update_position(); return TOK_PLUS; }
"-"   { update_position(); return TOK_MINUS; }
"*"   { update_position(); return TOK_MULT; }
"/"   { update_position(); return TOK_DIV_REAL; }
"div" { update_position(); return TOK_DIV; }
"mod" { update_position(); return TOK_MOD; }
"^"   { update_position(); return TOK_POWER; }

"(" { update_position(); return TOK_LPAREN; }
")" { update_position(); return TOK_RPAREN; }
"[" { update_position(); return TOK_LBRACKET; }
"]" { update_position(); return TOK_RBRACKET; }
"{" { update_position(); return TOK_LBRACE; }
"}" { update_position(); return TOK_RBRACE; }
"," { update_position(); return TOK_COMMA; }
":" { update_position(); return TOK_COLON; }
"." { update_position(); return TOK_DOT; }
"|" { update_position(); return TOK_PIPE; }
";" { update_position(); return TOK_SEMICOLON; }

"#".* { update_position(); }

{DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?i |
{DIGIT}+[eE][+-]?{DIGIT}+i |
{DIGIT}+\.([eE][+-]?{DIGIT}+)?i |
\.{DIGIT}+([eE][+-]?{DIGIT}+)?i |
{DIGIT}+i {
    yylval.strval = strdup(yytext);
    update_position();
    return TOK_COMPLEX;
}

{DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)? |
{DIGIT}+[eE][+-]?{DIGIT}+ |
{DIGIT}+\. |
\.{DIGIT}+ {
    yylval.floatval = atof(yytext);
    update_position();
    return TOK_FLOAT;
}

{DIGIT}+ {
    yylval.intval = atoi(yytext);
    update_position();
    return TOK_INT;
}

\"([^\"\\]|\\.)*\" {
    yylval.strval = strdup(yytext);
    update_position();
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
    update_position();
    return TOK_CHAR;
}

{ID_START}{ID_CHAR}* {
    yylval.strval = strdup(yytext);
    update_position();
    return TOK_ID;
}

{WHITESPACE}+ { update_position(); }
{NEWLINE}     { handle_newline(); }

. {
    fprintf(stderr, "Erreur lexicale à la ligne %d, colonne %d: caractère inconnu '%s'\n",
            line_num, col_num, yytext);
    update_position();
}

%%
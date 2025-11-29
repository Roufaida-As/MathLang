%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"

/* Variables globales pour le tracking */
int line_num = 1;
int col_num = 1;

/* Fonction pour mettre à jour la position dans le fichier */
void update_position() {
    col_num += yyleng;
}

/* Fonction pour gérer les nouvelles lignes */
void handle_newline() {
    line_num++;
    col_num = 1;
}
%}

/* ============================================ */
/* OPTIONS FLEX                                */
/* ============================================ */
%option noyywrap
%option yylineno

/* ============================================ */
/* DÉFINITIONS DE PATTERNS RÉUTILISABLES       */
/* ============================================ */
DIGIT       [0-9]
LETTER      [a-zA-Z]
ID_START    [a-zA-Z_]
ID_CHAR     [a-zA-Z0-9_]
WHITESPACE  [ \t\r]
NEWLINE     \n

%%
/* ============================================ */
/* SECTION 3 : RÈGLES D'ANALYSE LEXICALE       */
/* ============================================ */

/* --------------------MOTS-CLÉS -------------------- */

/* Mots-clés de déclaration */
"SOIT"              { update_position(); return TOK_SOIT; }
"SOIT_CONST"        { update_position(); return TOK_SOIT_CONST; }
"tel"[ \t]+"que"    { update_position(); return TOK_TEL_QUE; }
"TYPE"              { update_position(); return TOK_TYPE; }
"ENREGISTREMENT"    { update_position(); return TOK_ENREGISTREMENT; }

/* Structures de contrôle - SI/ALORS/SINON */
"SI"                { update_position(); return TOK_SI; }
"ALORS"             { update_position(); return TOK_ALORS; }
"SINON"             { update_position(); return TOK_SINON; }
"FIN"               { update_position(); return TOK_FIN; }

/* Structures de contrôle - TANT QUE */
"TANT"              { update_position(); return TOK_TANT; }
"QUE"               { update_position(); return TOK_QUE; }
"FAIRE"             { update_position(); return TOK_FAIRE; }

/* Structures de contrôle - POUR */
"POUR"              { update_position(); return TOK_POUR; }
"DE"                { update_position(); return TOK_DE; }
"A"                 { update_position(); return TOK_A; }
"PAR"               { update_position(); return TOK_PAR; }

/* Structures de contrôle - REPETER */
"REPETER"           { update_position(); return TOK_REPETER; }
"JUSQU'A"           { update_position(); return TOK_JUSQUA; }

/* Contrôle de flux */
"SORTIR"            { update_position(); return TOK_SORTIR; }
"CONTINUER"         { update_position(); return TOK_CONTINUER; }

/* Entrées/Sorties */
"AFFICHER"          { update_position(); return TOK_AFFICHER; }
"AFFICHER_LIGNE"    { update_position(); return TOK_AFFICHER_LIGNE; }
"LIRE"              { update_position(); return TOK_LIRE; }

/* Fonctions et procédures */
"FONCTION"          { update_position(); return TOK_FONCTION; }
"PROCEDURE"         { update_position(); return TOK_PROCEDURE; }
"RETOURNER"         { update_position(); return TOK_RETOURNER; }
"lambda"            { update_position(); return TOK_LAMBDA; }

/* Booléens */
"vrai"              { update_position(); return TOK_TRUE; }
"faux"              { update_position(); return TOK_FALSE; }

/* Opérateurs logiques textuels */
"et"                { update_position(); return TOK_AND; }
"ou"                { update_position(); return TOK_OR; }
"non"               { update_position(); return TOK_NOT; }
"xor"               { update_position(); return TOK_XOR; }

/* Types de base */
"Z"                 { update_position(); return TOK_TYPE_Z; }
"R"                 { update_position(); return TOK_TYPE_R; }
"B"                 { update_position(); return TOK_TYPE_B; }
"C"                 { update_position(); return TOK_TYPE_C; }
"Sigma"             { update_position(); return TOK_TYPE_SIGMA; }
"Char"              { update_position(); return TOK_TYPE_CHAR; }

/* Sous-types numériques */
"int8"              { update_position(); return TOK_INT8; }
"int16"             { update_position(); return TOK_INT16; }
"int32"             { update_position(); return TOK_INT32; }
"int64"             { update_position(); return TOK_INT64; }
"float"             { update_position(); return TOK_FLOAT_TYPE; }
"double"            { update_position(); return TOK_DOUBLE_TYPE; }

/* Mots-clés DANS/dans (IMPORTANT: accepter les DEUX selon rapport) */
"DANS"              { update_position(); return TOK_IN; }
"dans"              { update_position(); return TOK_IN; }


/* --------------------OPÉRATEURS -------------------- */

"<=>"       { update_position(); return TOK_EQUIV; }

"<-"        { update_position(); return TOK_ASSIGN; }
"->"        { update_position(); return TOK_ARROW; }
"=>"        { update_position(); return TOK_IMPLIES; }

"!="        { update_position(); return TOK_NEQ; }
"<="        { update_position(); return TOK_LEQ; }
">="        { update_position(); return TOK_GEQ; }

"="         { update_position(); return TOK_EQ; }
"<"         { update_position(); return TOK_LT; }
">"         { update_position(); return TOK_GT; }

"+"         { update_position(); return TOK_PLUS; }
"-"         { update_position(); return TOK_MINUS; }
"*"         { update_position(); return TOK_MULT; }
"/"         { update_position(); return TOK_DIV_REAL; }
"^"         { update_position(); return TOK_POWER; }

"div"       { update_position(); return TOK_DIV; }
"mod"       { update_position(); return TOK_MOD; }

"union"     { update_position(); return TOK_UNION; }
"inter"     { update_position(); return TOK_INTER; }
"diff"      { update_position(); return TOK_DIFF; }
"\\"        { update_position(); return TOK_DIFF; }
"symdiff"   { update_position(); return TOK_SYMDIFF; }

"inclus"    { update_position(); return TOK_SUBSET; }
"strict_inclus" { update_position(); return TOK_STRICT_SUBSET; }

"("         { update_position(); return TOK_LPAREN; }
")"         { update_position(); return TOK_RPAREN; }
"["         { update_position(); return TOK_LBRACKET; }
"]"         { update_position(); return TOK_RBRACKET; }
"{"         { update_position(); return TOK_LBRACE; }
"}"         { update_position(); return TOK_RBRACE; }
","         { update_position(); return TOK_COMMA; }
":"         { update_position(); return TOK_COLON; }
"."         { update_position(); return TOK_DOT; }
"|"         { update_position(); return TOK_PIPE; }
";"         { update_position(); return TOK_SEMICOLON; }


/* -------------------- LITTÉRAUX -------------------- */

/* Commentaires */
"#".*       { update_position(); }

/* Nombres complexes */
{DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?i   { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_COMPLEX; 
}

{DIGIT}+[eE][+-]?{DIGIT}+i                { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_COMPLEX; 
}

{DIGIT}+\.([eE][+-]?{DIGIT}+)?i           { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_COMPLEX; 
}

\.{DIGIT}+([eE][+-]?{DIGIT}+)?i           { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_COMPLEX; 
}

{DIGIT}+i                                  { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_COMPLEX; 
}

/* Nombres réels */
{DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?    { 
    yylval.floatval = atof(yytext); 
    update_position();
    return TOK_FLOAT; 
}
{DIGIT}+[eE][+-]?{DIGIT}+                 { 
    yylval.floatval = atof(yytext); 
    update_position();
    return TOK_FLOAT; 
}
{DIGIT}+\.([eE][+-]?{DIGIT}+)?            { 
    yylval.floatval = atof(yytext); 
    update_position();
    return TOK_FLOAT; 
}
\.{DIGIT}+([eE][+-]?{DIGIT}+)?            { 
    yylval.floatval = atof(yytext); 
    update_position();
    return TOK_FLOAT; 
}

/* Nombres entiers */
{DIGIT}+                                   { 
    yylval.intval = atoi(yytext); 
    update_position();
    return TOK_INT; 
}

/* Chaînes de caractères */
\"([^\"\\]|\\.)*\"                         { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_STRING; 
}

/* Caractères */
\'([^\'\\]|\\.)\'                          { 
    /* Gestion des caractères échappés */
    if (yytext[1] == '\\') {
        switch(yytext[2]) {
            case 'n':  yylval.charval = '\n'; break;
            case 't':  yylval.charval = '\t'; break;
            case 'r':  yylval.charval = '\r'; break;
            case '\\': yylval.charval = '\\'; break;
            case '\'': yylval.charval = '\''; break;
            case '0':  yylval.charval = '\0'; break;
            default:   yylval.charval = yytext[2]; break;
        }
    } else {
        yylval.charval = yytext[1];
    }
    update_position();
    return TOK_CHAR; 
}

/* Identificateurs (EN DERNIER car pattern général) */
{ID_START}{ID_CHAR}{0,62}                  { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_ID; 
}

/* Espaces blancs */
{WHITESPACE}+   { update_position(); }
{NEWLINE}       { handle_newline(); }

/* Gestion d'erreurs (catchall) */
.   { 
    update_position();
    return TOK_ERROR;
}

%%
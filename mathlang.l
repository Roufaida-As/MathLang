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
"\\"         { update_position(); return TOK_DIFF; }
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



"#".*                          { update_position();  }


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



{DIGIT}+                                   { 
    yylval.intval = atoi(yytext); 
    update_position();
    return TOK_INT; 
}



\"([^\"\\]|\\.)*\"                         { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_STRING; 
}



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



{ID_START}{ID_CHAR}{0,62}                  { 
    yylval.strval = strdup(yytext); 
    update_position();
    return TOK_ID; 
}


{WHITESPACE}+   { update_position(); }
{NEWLINE}       { handle_newline(); }


.   { 
    update_position();
    return TOK_ERROR;
}

%%
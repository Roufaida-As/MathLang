#ifndef TOKENS_H
#define TOKENS_H

/* ============================================ */
/* DÉFINITION DES TOKENS POUR MATHLANG        */
/* ============================================ */

/* Structure pour stocker les valeurs des tokens */
typedef union {
    int intval;
    double floatval;
    char charval;
    char* strval;
} YYSTYPE;

extern YYSTYPE yylval;

/* ============================================ */
/* MOTS-CLÉS - DÉCLARATIONS                    */
/* ============================================ */
#define TOK_SOIT            100
#define TOK_SOIT_CONST      101
#define TOK_DANS            102
#define TOK_TEL_QUE         103
#define TOK_TYPE            104
#define TOK_ENREGISTREMENT  105

/* ============================================ */
/* MOTS-CLÉS - STRUCTURES DE CONTRÔLE          */
/* ============================================ */
#define TOK_SI              110
#define TOK_ALORS           111
#define TOK_SINON           112
#define TOK_FIN             113
#define TOK_TANT            114
#define TOK_QUE             115
#define TOK_FAIRE           116
#define TOK_POUR            117
#define TOK_DE              118
#define TOK_A               119
#define TOK_PAR             120
#define TOK_REPETER         121
#define TOK_JUSQUA          122

/* ============================================ */
/* MOTS-CLÉS - CONTRÔLE DE FLUX                */
/* ============================================ */
#define TOK_SORTIR          130
#define TOK_CONTINUER       131

/* ============================================ */
/* MOTS-CLÉS - ENTRÉES/SORTIES                 */
/* ============================================ */
#define TOK_AFFICHER        140
#define TOK_AFFICHER_LIGNE  141
#define TOK_LIRE            142

/* ============================================ */
/* MOTS-CLÉS - FONCTIONS ET PROCÉDURES         */
/* ============================================ */
#define TOK_FONCTION        150
#define TOK_PROCEDURE       151
#define TOK_RETOURNER       152
#define TOK_LAMBDA          153

/* ============================================ */
/* CONSTANTES BOOLÉENNES                       */
/* ============================================ */
#define TOK_TRUE            160
#define TOK_FALSE           161

/* ============================================ */
/* OPÉRATEURS LOGIQUES (TEXTUELS)              */
/* ============================================ */
#define TOK_AND             170
#define TOK_OR              171
#define TOK_NOT             172
#define TOK_XOR             173

/* ============================================ */
/* TYPES DE BASE                               */
/* ============================================ */
#define TOK_TYPE_Z          180  /* Entiers */
#define TOK_TYPE_R          181  /* Réels */
#define TOK_TYPE_B          182  /* Booléens */
#define TOK_TYPE_C          183  /* Complexes */
#define TOK_TYPE_SIGMA      184  /* Chaînes */
#define TOK_TYPE_CHAR       185  /* Caractères */

/* ============================================ */
/* SOUS-TYPES NUMÉRIQUES                       */
/* ============================================ */
#define TOK_INT8            190
#define TOK_INT16           191
#define TOK_INT32           192
#define TOK_INT64           193
#define TOK_FLOAT_TYPE      194
#define TOK_DOUBLE_TYPE     195

/* ============================================ */
/* OPÉRATEUR D'AFFECTATION                     */
/* ============================================ */
#define TOK_ASSIGN          200  /* <- */

/* ============================================ */
/* OPÉRATEURS ARITHMÉTIQUES                    */
/* ============================================ */
#define TOK_PLUS            210  /* + */
#define TOK_MINUS           211  /* - */
#define TOK_MULT            212  /* * */
#define TOK_DIV_REAL        213  /* / (division réelle) */
#define TOK_DIV             214  /* div (division entière) */
#define TOK_MOD             215  /* mod (modulo) */
#define TOK_POWER           216  /* ^ (puissance) */

/* ============================================ */
/* OPÉRATEURS DE COMPARAISON                   */
/* ============================================ */
#define TOK_EQ              220  /* = */
#define TOK_NEQ             221  /* != ou ≠ */
#define TOK_LT              222  /* < */
#define TOK_GT              223  /* > */
#define TOK_LEQ             224  /* <= ou ≤ */
#define TOK_GEQ             225  /* >= ou ≥ */

/* ============================================ */
/* OPÉRATEURS ENSEMBLISTES                     */
/* ============================================ */
#define TOK_UNION           230  /* union ou ∪ */
#define TOK_INTER           231  /* inter ou ∩ */
#define TOK_DIFF            232  /* diff ou \ */
#define TOK_SYMDIFF         233  /* △ (différence symétrique) */

/* ============================================ */
/* OPÉRATEURS D'APPARTENANCE ET INCLUSION      */
/* ============================================ */
#define TOK_IN              240  /* dans ou ∈ */
#define TOK_NOT_IN          241  /* /∈ */
#define TOK_SUBSET          242  /* inclus ou ⊆ */
#define TOK_STRICT_SUBSET   243  /* ⊂ */

/* ============================================ */
/* DÉLIMITEURS                                 */
/* ============================================ */
#define TOK_LPAREN          250  /* ( */
#define TOK_RPAREN          251  /* ) */
#define TOK_LBRACKET        252  /* [ */
#define TOK_RBRACKET        253  /* ] */
#define TOK_LBRACE          254  /* { */
#define TOK_RBRACE          255  /* } */
#define TOK_COMMA           256  /* , */
#define TOK_COLON           257  /* : */
#define TOK_DOT             258  /* . */
#define TOK_PIPE            259  /* | */
#define TOK_SEMICOLON       260  /* ; */

/* ============================================ */
/* SYMBOLES SPÉCIAUX                           */
/* ============================================ */
#define TOK_ARROW           270  /* -> */
#define TOK_IMPLIES         271  /* => ou ⇒ */
#define TOK_EQUIV           272  /* <=> ou ⇔ */

/* ============================================ */
/* LITTÉRAUX - VALEURS                         */
/* ============================================ */
#define TOK_INT             300  /* Nombre entier : 42 */
#define TOK_FLOAT           301  /* Nombre réel : 3.14 */
#define TOK_STRING          302  /* Chaîne : "texte" */
#define TOK_CHAR            303  /* Caractère : 'a' */
#define TOK_COMPLEX         304  /* Nombre complexe : 2+3i */

/* ============================================ */
/* IDENTIFICATEURS                             */
/* ============================================ */
#define TOK_ID              310  /* nom_variable, x, somme, etc. */

/* ============================================ */
/* FONCTIONS MATHÉMATIQUES (OPTIONNEL - we should decide if we'll do them)         */
/* ============================================ */
#define TOK_SIN             400
#define TOK_COS             401
#define TOK_EXP             402
#define TOK_LOG             403
#define TOK_SQRT            404
#define TOK_ABS             405
#define TOK_FLOOR           406
#define TOK_CEIL            407
#define TOK_ROUND           408

/* Fonctions de conversion */
#define TOK_INT_FUNC        410
#define TOK_REEL            411
#define TOK_BOOLEEN         412
#define TOK_CHAINE          413

/* Fonctions complexes */
#define TOK_RE              420  /* Re(z) - partie réelle */
#define TOK_IM              421  /* Im(z) - partie imaginaire */
#define TOK_ARG             422  /* arg(z) - argument */

/* Fonctions sur chaînes */
#define TOK_MAJUSCULES      430
#define TOK_MINUSCULES      431
#define TOK_DIVISER         432

/* Fonctions matricielles */
#define TOK_DET             440  /* determinant */
#define TOK_INV             441  /* inverse */
#define TOK_TRACE           442
#define TOK_IDENTITE        443

/* ============================================ */
/* TOKENS SPÉCIAUX                             */
/* ============================================ */
#define TOK_EOF             900  /* Fin de fichier */
#define TOK_ERROR           999  /* Erreur lexicale */


/**
 * Retourne le nom lisible d'un token
 * @param token Le code du token
 * @return Le nom du token sous forme de chaîne
 */
const char* token_name(int token);

#endif 
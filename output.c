/* ===================================================== */
/* Fichier genere automatiquement par le compilateur      */
/* MathLang -> C. Ne pas modifier a la main.              */
/* ===================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <complex.h>

/* --- Variables globales declarees dans MathLang --- */
long quotient_entier;
long a;
long compteur;
long b;
double complex imag;
long i;
long j;
char* texte = NULL;
double r;
double x;
double complex c_produit;
long entier;
int ACTIF;
long imbrique;
char* nom = NULL;
long f5;
long reste;
double sc;
char* nom_maj = NULL;
double complex c_somme;
long dif;
long produit;
char* salutation = NULL;
int cond1;
int cond2;
double complex c1;
int cond3;
double complex c2;
double puissance;
double quotient_reel;
long valeur_lue;
char* nom_min = NULL;
long note;
int bo;
long somme;
double PI;
char lettre;

/* --- Prototypes --- */
long factorielle(long n);
double somme_carres(double a, double b);
void afficher_titre(char* titre);

long factorielle(long n) {
    /* --- Temporaires / variables locales --- */
    long T0;
    long T1;
    long T2;

    if ((n) > (1)) goto L3;
    return 1;
    goto L8;
L3:;
    T0 = n - 1;
    T1 = factorielle(T0);
    T2 = n * T1;
    return T2;
L8:;
    /* filet de securite si aucun RETOURNER n'est atteint */
    return (long)0;
}

double somme_carres(double a, double b) {
    /* --- Temporaires / variables locales --- */
    double T3;
    double T4;
    double T5;

L8:;
    T3 = a * a;
    T4 = b * b;
    T5 = T3 + T4;
    return T5;
    /* filet de securite si aucun RETOURNER n'est atteint */
    return (double)0;
}

void afficher_titre(char* titre) {
    /* --- Temporaires / variables locales --- */

    printf("%s", titre);
    printf("\n");
    printf("%s", "-------------------------");
    printf("%s", "");
    printf("\n");
    return;
}

int main(void) {
    /* --- Temporaires / variables locales --- */
    long T6;
    long T7;
    long T8;
    double T9;
    long T10;
    long T11;
    long T12;
    long T13;
    long T14;
    long k;
    long T15;
    long T16;
    long T17;
    double T18;
    double T19;
    double T20;
    double T21;
    double T22;
    double T23;
    double T24;
    double T25;
    double T26;
    double complex T27;
    double complex T28;
    double T29;
    double T30;
    double T31;
    double T32;
    double complex T33;
    char* T34;
    char* T35;
    char* T36;
    long T37;
    double T38;
    long T39;
    long T40;
    long T41;

    entier = 42;
    r = 3.14;
    bo = true;
    lettre = 'x';
    texte = "Bonjour";
    imag = (3*I);
    PI = 3.14159;
    ACTIF = true;
    afficher_titre("Test arithmetique");
    a = 10;
    b = 3;
    printf("%s", "a =");
    printf("%ld", a);
    printf("\n");
    printf("%s", "b =");
    printf("%ld", b);
    printf("\n");
    T6 = a + b;
    somme = T6;
    T7 = a - b;
    dif = T7;
    T8 = a * b;
    produit = T8;
    T9 = (double)(a) / (double)(b);
    quotient_reel = T9;
    T10 = (long)(a) / (long)(b);
    quotient_entier = T10;
    T11 = (long)(a) % (long)(b);
    reste = T11;
    T12 = pow((double)(a), (double)(2));
    puissance = T12;
    printf("%s", "a + b = ");
    printf("%ld", somme);
    printf("\n");
    printf("%s", "a - b = ");
    printf("%ld", dif);
    printf("\n");
    printf("%s", "a * b = ");
    printf("%ld", produit);
    printf("\n");
    printf("%s", "a / b = ");
    printf("%g", quotient_reel);
    printf("\n");
    printf("%s", "a div b = ");
    printf("%ld", quotient_entier);
    printf("\n");
    printf("%s", "a mod b = ");
    printf("%ld", reste);
    printf("\n");
    printf("%s", "a ^ 2 = ");
    printf("%g", puissance);
    printf("\n");
    cond1 = 0;
    cond2 = 0;
    cond3 = true;
    printf("%s", "cond1 = ");
    printf("%s", (cond1) ? "true" : "false");
    printf("\n");
    printf("%s", "cond2 = ");
    printf("%s", (cond2) ? "true" : "false");
    printf("\n");
    printf("%s", "cond3 = ");
    printf("%s", (cond3) ? "true" : "false");
    printf("\n");
    afficher_titre("Test SI / SINON SI / SINON");
    note = 14;
    if ((note) < (16)) goto L89;
    printf("%s", "Mention Tres Bien");
    printf("\n");
    goto L99;
L89:;
    if ((note) < (14)) goto L93;
    printf("%s", "Mention Bien");
    printf("\n");
    goto L99;
L93:;
    if ((note) < (12)) goto L97;
    printf("%s", "Mention Assez Bien");
    printf("\n");
    goto L99;
L97:;
    printf("%s", "Pas de mention");
    printf("\n");
L99:;
    afficher_titre("Test TANT QUE avec SORTIR/CONTINUER");
    i = 0;
L102:;
    if ((i) >= (20)) goto L114;
    T13 = i + 1;
    i = T13;
    T14 = (long)(i) % (long)(2);
    if ((T14) != (0)) goto L108;
    goto L102;
L108:;
    if ((i) <= (15)) goto L110;
    goto L114;
L110:;
    printf("%s", "impair retenu : ");
    printf("%ld", i);
    printf("\n");
    goto L102;
L114:;
    afficher_titre("Test POUR SImple");
    k = 1;
L117:;
    if ((k) > (5)) goto L124;
    printf("%s", "k = ");
    printf("%ld", k);
    printf("\n");
    T15 = k + 1;
    k = T15;
    goto L117;
L124:;
    afficher_titre("Test POUR avec PAR (pas de 2)");
    k = 0;
L127:;
    if ((k) > (10)) goto L134;
    printf("%s", "k (pas 2) = ");
    printf("%ld", k);
    printf("\n");
    T16 = k + 2;
    k = T16;
    goto L127;
L134:;
    afficher_titre("Test REPETER / JUSQUA");
    j = 0;
L137:;
    T17 = j + 1;
    j = T17;
    printf("%s", "j = ");
    printf("%ld", j);
    printf("\n");
    if ((j) < (5)) goto L137;
    afficher_titre("Test fonctions mathematiques");
    x = 2;
    printf("%s", "sin(x) = ");
    T18 = sin((double)(x));
    printf("%g", T18);
    printf("\n");
    printf("%s", "cos(x) = ");
    T19 = cos((double)(x));
    printf("%g", T19);
    printf("\n");
    printf("%s", "exp(x) = ");
    T20 = exp((double)(x));
    printf("%g", T20);
    printf("\n");
    printf("%s", "log(x) = ");
    T21 = log((double)(x));
    printf("%g", T21);
    printf("\n");
    printf("%s", "sqrt(x) = ");
    T22 = sqrt((double)(x));
    printf("%g", T22);
    printf("\n");
    printf("%s", "abs(x) = ");
    T23 = fabs((double)(x));
    printf("%g", T23);
    printf("\n");
    printf("%s", "floor(3.7) = ");
    T24 = floor((double)(3.7));
    printf("%g", T24);
    printf("\n");
    printf("%s", "ceil(3.2) = ");
    T25 = ceil((double)(3.2));
    printf("%g", T25);
    printf("\n");
    printf("%s", "round(3.5) = ");
    T26 = round((double)(3.5));
    printf("%g", T26);
    printf("\n");
    afficher_titre("Test nombres complexes");
    c1 = (3*I);
    c2 = (2.5*I);
    T27 = c1 + c2;
    c_somme = T27;
    T28 = c1 * c2;
    c_produit = T28;
    printf("%s", "im(c1) = ");
    T29 = cimag(c1);
    printf("%g", T29);
    printf("\n");
    printf("%s", "re(c1) = ");
    T30 = creal(c1);
    printf("%g", T30);
    printf("\n");
    printf("%s", "arg(c1) = ");
    T31 = carg(c1);
    printf("%g", T31);
    printf("\n");
    printf("%s", "sqrt(c1) = ");
    T32 = csqrt(c1);
    printf("%g", T32);
    printf("\n");
    printf("%s", "abs(c1) = ");
    T33 = cabs(c1);
    printf("%g", (double)(T33));
    printf("\n");
    afficher_titre("Test chaines de caracteres");
    nom = "mathlang";
    T34 = strdup(nom);
    for (char* p = T34; *p; p++) *p = (char)toupper((unsigned char)*p);
    nom_maj = T34;
    T35 = strdup(nom_maj);
    for (char* p = T35; *p; p++) *p = (char)tolower((unsigned char)*p);
    nom_min = T35;
    printf("%s", nom_maj);
    printf("\n");
    printf("%s", nom_min);
    printf("\n");
    T36 = malloc(strlen("Bonjour, ") + strlen(nom) + 1);
    strcpy(T36, "Bonjour, ");
    strcat(T36, nom);
    salutation = T36;
    printf("%s", salutation);
    printf("\n");
    afficher_titre("Test lecture clavier");
    printf("%s", "Entrez un entier :");
    printf("\n");
    scanf("%ld", &valeur_lue);
    printf("%s", "Vous avez entre : ");
    printf("%ld", valeur_lue);
    printf("\n");
    afficher_titre("Test appels de fonctions");
    T37 = factorielle(5);
    f5 = T37;
    printf("%s", "factorielle(5) = ");
    printf("%ld", f5);
    printf("\n");
    T38 = somme_carres(3, 4);
    sc = T38;
    printf("%s", "somme_carres(3,4) = ");
    printf("%g", sc);
    printf("\n");
    T39 = factorielle(4);
    T40 = T39 - 5;
    T41 = factorielle(T40);
    imbrique = T41;
    printf("%s", "appel imbrique = ");
    printf("%ld", imbrique);
    printf("\n");
    printf("%s", "=== FIN DES TESTS ===");
    printf("\n");
    return 0;
}

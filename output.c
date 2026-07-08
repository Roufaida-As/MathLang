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

int main(void) {
    /* --- Variables declarees dans MathLang --- */
    double a;
    long b;
    double complex c;
    long d;
    char ch;
    char* s;
    s = NULL;
    long x;
    double y;
    double complex z;
    char* msg;
    msg = NULL;
    char* up;
    up = NULL;
    char* low;
    low = NULL;

    /* --- Temporaires du compilateur et variables dont la portee --- */
    /* --- avait deja ete fermee au moment du codegen           --- */
    double complex T0;
    double complex T1;
    double T2;
    double T3;
    long T4;
    double T5;
    double T6;
    long T7;
    long T8;
    long i;
    long T9;
    long T10;
    double T11;
    char* T12;
    char* T13;

L0:;
    x = 10;
L1:;
    y = 3;
L2:;
    T0 = 2.5 - (3*I);
L3:;
    z = T0;
L4:;
    T1 = 3 + (4.0*I);
L5:;
    c = T1;
L6:;
    if (!(true)) goto L9;
L7:;
    printf("%s", "Boolean logic OK");
L8:;
    printf("\n");
L9:;
    T2 = y * 2;
L10:;
    T3 = x + T2;
L11:;
    T4 = pow((double)(3), (double)(2));
L12:;
    T5 = (double)(5) / (double)(T4);
L13:;
    T6 = T3 - T5;
L14:;
    a = T6;
L15:;
    T7 = (long)(x) / (long)(3);
L16:;
    b = T7;
L17:;
    T8 = (long)(x) % (long)(4);
L18:;
    d = T8;
L19:;
    if ((x) < (10)) goto L22;
L20:;
    printf("%s", "Comparisons OK");
L21:;
    printf("\n");
L22:;
    i = 1;
L23:;
    if ((i) > (10)) goto L29;
L24:;
    printf("%ld", i);
L25:;
    printf("\n");
L26:;
    T9 = i + 2;
L27:;
    i = T9;
L28:;
    goto L23;
L29:;
    if ((x) <= (0)) goto L33;
L30:;
    T10 = x - 1;
L31:;
    x = T10;
L32:;
    goto L29;
L33:;
    T11 = y + 1;
L34:;
    y = T11;
L35:;
    if ((y) != (10)) goto L33;
L36:;
    msg = "Hello World";
L37:;
    T12 = strdup(msg);
    for (char* p = T12; *p; p++) *p = (char)toupper((unsigned char)*p);
L38:;
    up = T12;
L39:;
    T13 = strdup(msg);
    for (char* p = T13; *p; p++) *p = (char)tolower((unsigned char)*p);
L40:;
    low = T13;
L41:;
    s = "string with \"escape\"";
L42:;
    ch = 'a';
L43:;
    return 0;
}

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

int main(void) {
    /* --- Variables declarees dans MathLang --- */
    double a;
    long b;
    double c;
    long d;
    char ch;
    char* s;
    s = NULL;
    long x;
    double y;
    double z;
    char* msg;
    msg = NULL;
    char* up;
    up = NULL;
    char* low;
    low = NULL;

    /* --- Temporaires du compilateur et variables dont la portee --- */
    /* --- avait deja ete fermee au moment du codegen           --- */
    double T0;
    long T1;
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
    y = 3.14;
L2:;
    T0 = 2.5 - 3i;
L3:;
    z = T0;
L4:;
    T1 = 3 + 4.0i;
L5:;
    c = T1;
L6:;
    if (!(true)) goto L8;
L7:;
    printf("%s", "Boolean logic OK");
L8:;
    T2 = y * 2;
L9:;
    T3 = x + T2;
L10:;
    T4 = pow((double)(3), (double)(2));
L11:;
    T5 = (double)(5) / (double)(T4);
L12:;
    T6 = T3 - T5;
L13:;
    a = T6;
L14:;
    T7 = (long)(x) / (long)(3);
L15:;
    b = T7;
L16:;
    T8 = (long)(x) % (long)(4);
L17:;
    d = T8;
L18:;
    if ((x) < (10)) goto L20;
L19:;
    printf("%s", "Comparisons OK");
L20:;
    i = 1;
L21:;
    if ((i) > (10)) goto L26;
L22:;
    printf("%ld", i);
L23:;
    T9 = i + 2;
L24:;
    i = T9;
L25:;
    goto L21;
L26:;
    if ((x) <= (0)) goto L30;
L27:;
    T10 = x - 1;
L28:;
    x = T10;
L29:;
    goto L26;
L30:;
    T11 = y + 1;
L31:;
    y = T11;
L32:;
    if ((y) != (10)) goto L30;
L33:;
    msg = "Hello World";
L34:;
    T12 = strdup(msg);
    for (char* p = T12; *p; p++) *p = (char)toupper((unsigned char)*p);
L35:;
    up = T12;
L36:;
    T13 = strdup(msg);
    for (char* p = T13; *p; p++) *p = (char)tolower((unsigned char)*p);
L37:;
    low = T13;
L38:;
    s = "string with \"escape\"";
L39:;
    ch = 'a';
L40:;
    return 0;
}

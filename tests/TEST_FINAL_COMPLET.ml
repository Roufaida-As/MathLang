# ============================================
# TEST COMPLET - MATHLANG LEXER
# Sections A + B + C
# ============================================

# === SECTION A : MOTS-CLÉS ET DÉCLARATIONS ===

SOIT x dans Z tel que x <- 5
SOIT y dans R tel que y <- 3.14159
SOIT flag dans B tel que flag <- vrai
SOIT nom dans Sigma tel que nom <- "Alice"
SOIT lettre dans Char tel que lettre <- 'A'

SOIT_CONST PI dans R tel que PI <- 3.14159265359

TYPE Point = ENREGISTREMENT
    x : R
    y : R
FIN

# === STRUCTURES DE CONTRÔLE ===

SI x > 0 ALORS
    AFFICHER("x est positif")
SINON SI x < 0 ALORS
    AFFICHER("x est negatif")
SINON
    AFFICHER("x est nul")
FIN

# === BOUCLES ===

POUR i DE 0 A 10 PAR 2 FAIRE
    AFFICHER(i)
FIN

TANT QUE x > 0 FAIRE
    x <- x - 1
FIN

REPETER
    AFFICHER("Entrez un nombre positif")
    LIRE(n)
JUSQUA n > 0

# === CONTRÔLE DE FLUX ===

POUR i DE 0 A 100 FAIRE
    SI i = 50 ALORS
        SORTIR
    FIN
    SI i mod 2 = 0 ALORS
        CONTINUER
    FIN
    AFFICHER(i)
FIN

# === FONCTIONS ===

FONCTION carre(x dans Z) dans Z
    RETOURNER x * x
FIN

PROCEDURE afficher_message(msg dans Sigma)
    AFFICHER(msg)
FIN

double <- lambda x . x * 2

# === BOOLÉENS ET LOGIQUE ===

condition1 <- vrai
condition2 <- faux
resultat1 <- condition1 et condition2
resultat2 <- condition1 ou condition2
resultat3 <- non condition1
resultat4 <- condition1 xor condition2

# === TYPES ===

entier_z <- 42
reel_r <- 3.14
booleen_b <- vrai
chaine_sigma <- "Hello"
caractere_char <- 'Z'

int8_var <- 127
int16_var <- 32767
int32_var <- 2147483647
int64_var <- 9223372036854775807

float_var <- 1.5
double_var <- 2.718281828

# === SECTION B : OPÉRATEURS ===

# Affectation
resultat <- 0

# Arithmétiques
addition <- 10 + 20
soustraction <- 50 - 30
multiplication <- 5 * 6
division_reelle <- 10 / 3
division_entiere <- 10 div 3
modulo <- 17 mod 5
puissance <- 2 ^ 8

# Comparaisons
egal <- x = y
different <- x != y
inferieur <- x < y
superieur <- x > y
inf_ou_egal <- x <= y
sup_ou_egal <- x >= y

# Ensembles
A <- 1
B <- 2
C <- 3

union_set <- A union B
inter_set <- A inter B
diff_set1 <- A diff B
diff_set2 <- A \ B
symdiff_set <- A symdiff B

# Appartenance et inclusion
appartient <- x dans E
inclus_set <- A inclus B
strict_inclus_set <- A strict_inclus B

# Délimiteurs
tableau[0] <- 100
tableau[1] <- 200
matrice[1,2] <- 3.14
fonction(a, b, c)
ensemble <- {1, 2, 3, 4, 5}
point.x <- 10
point.y <- 20
filtre <- condition | action
instruction1; instruction2; instruction3;

# Symboles spéciaux
fleche1 <- x -> y
implication <- p => q
equivalence <- a <=> b

# === SECTION C : LITTÉRAUX ===

# Nombres complexes
complexe1 <- 3i
complexe2 <- 2.5i
complexe3 <- 1.5e-3i
complexe4 <- 2+3i
complexe5 <- 5-2i

# Nombres réels
reel1 <- 3.14159
reel2 <- 0.5
reel3 <- .75
reel4 <- 123.456

# Notation scientifique
sci1 <- 1.5e10
sci2 <- 2.5e-3
sci3 <- 1.0E+5
sci4 <- 3.0E-2
sci5 <- 5e3
sci6 <- .5e-2

# Nombres entiers
entier1 <- 0
entier2 <- 42
entier3 <- 999999
entier4 <- 123456789

# Chaînes de caractères
chaine1 <- "Bonjour MathLang!"
chaine2 <- "Ceci est un test"
chaine3 <- "Caracteres speciaux : \n \t \\ \""
chaine4 <- ""

# Caractères
char1 <- 'a'
char2 <- 'Z'
char3 <- '5'


# Identificateurs variés
x <- 1
nom_variable <- 3
autreVariable <- 4
VAR_MAJUSCULE <- 5
_underscore <- 6
test123 <- 7
var_123_test <- 8

# Identificateur long (63 caractères max)
identificateur_tres_long_avec_63_caracteres_exactement_ici_ok <- 9

# === COMMENTAIRES ===

# Ceci est un commentaire complet
x <- 10  # Commentaire en fin de ligne

# Plusieurs commentaires
# sur plusieurs lignes
# pour tester la robustesse

# === TEST D'EXPRESSIONS COMPLEXES ===

expression1 <- (10 + 20) * (30 - 15) / 5
expression2 <- 2 ^ 3 ^ 2
expression3 <- ((a + b) * c - d) / (e + f)
expression4 <- x >= 0 et x <= 100
expression5 <- non (a ou b) et c
expression6 <- (a = b) ou (c != d) et (e < f)

# === TEST COMBINÉ ===

POUR i dans [0, 10[ FAIRE
    SI i mod 2 = 0 ALORS
        AFFICHER("Pair : ", i)
    SINON
        AFFICHER("Impair : ", i)
    FIN
FIN

TANT QUE condition1 et condition2 FAIRE
    x <- x + 1
    y <- y * 2
    SI x > 100 ou y > 1000 ALORS
        SORTIR
    FIN
FIN

# === FIN DU TEST ===
AFFICHER("Test lexical complet termine !")


# =====================================================================
#  TEST COMPLET DU COMPILATEUR MATHLANG
#   =====================================================================
#   Ce fichier exerce :
#     - declarations de variables / constantes (tous les types de base)
#    - operateurs arithmetiques, logiques, de comparaison
#    - structures de controle : SI/SINON SI/SINON, TANT QUE, POUR (avec
#     et sans PAR), REPETER/JUSQUA, SORTIR, CONTINUER
#   - fonctions mathematiques (SIN, COS, EXP, LOG, SQRT, ABS, FLOOR,
#    CEIL, ROUND, RE, IM, ARG)
#  - fonctions sur les chaines (MAJUSCULES, MINUSCULES, concatenation)
#  - nombres complexes
# - lecture/ecriture (LIRE, AFFICHER, AFFICHER_LIGNE)
# - fonctions et procedures, y compris appel recurSIf
# NOTE : ajustez les mots-cles ci-dessous SI l'orthographe exacte de
#  votre lexer differe (accents, casse, symboles).
# ===================================================================== 


# ---------------------------------------------------------------------
# 1) FONCTIONS ET PROCEDURES (deFINies avant leur premier appel, car
#      le compilateur est mono-passe)
#  ---------------------------------------------------------------------

# Fonction recurSIve : factorielle
FONCTION factorielle(n : Z) : Z
    SI n <= 1 ALORS
        RETOURNER 1
    SINON
        RETOURNER n * factorielle(n - 1)
    FIN
FIN

# Fonction avec deux parametres, sans recurSIvite 
FONCTION somme_carres(a : R, b : R) : R
    RETOURNER (a * a) + (b * b)
FIN

# Procedure : ne retourne rien, juste des effets de bord (affichage) */
PROCEDURE afficher_titre(titre : Sigma)
    AFFICHER_LIGNE(titre)
    AFFICHER("-------------------------")
    AFFICHER_LIGNE("")
FIN


# ---------------------------------------------------------------------
#   2) DECLARATIONS DE VARIABLES ET CONSTANTES : tous les types de base
#   --------------------------------------------------------------------- */

SOIT entier dans Z tel que entier <- 42
SOIT r dans R tel que r <- 3.14
SOIT bo dans B tel que bo <- vrai
SOIT lettre dans Char tel que lettre <- 'x'
SOIT texte dans Sigma tel que texte <- "Bonjour"
SOIT imag dans C tel que imag <- 3i

SOIT_CONST PI dans R tel que PI <- 3.14159
SOIT_CONST ACTIF dans B tel que ACTIF <- vrai

# Variable SImple sans initialisation immediate */
SOIT compteur dans Z


# ---------------------------------------------------------------------
#   3) ARITHMETIQUE, COMPARAISONS, LOGIQUE
#   --------------------------------------------------------------------- */

afficher_titre("Test arithmetique")

SOIT a dans Z tel que a <- 10
SOIT b dans Z tel que b <- 3

AFFICHER("a =") AFFICHER_LIGNE(a)
AFFICHER("b =") AFFICHER_LIGNE(b)

SOIT somme dans Z
somme <- a + b
SOIT dif dans Z
dif <- a - b
SOIT produit dans Z
produit <- a * b
SOIT quotient_reel dans R
quotient_reel <- a / b
SOIT quotient_entier dans Z
quotient_entier <- a div b
SOIT reste dans Z
reste <- a mod b
SOIT puissance dans R
puissance <- a ^ 2

AFFICHER("a + b = ") AFFICHER_LIGNE(somme)
AFFICHER("a - b = ") AFFICHER_LIGNE(dif)
AFFICHER("a * b = ") AFFICHER_LIGNE(produit)
AFFICHER("a / b = ") AFFICHER_LIGNE(quotient_reel)
AFFICHER("a div b = ") AFFICHER_LIGNE(quotient_entier)
AFFICHER("a mod b = ") AFFICHER_LIGNE(reste)
AFFICHER("a ^ 2 = ") AFFICHER_LIGNE(puissance)

SOIT cond1 dans B
cond1 <- (a > b) et (b > 0)
SOIT cond2 dans B
cond2 <- (a = b) ou non (b = 0)
SOIT cond3 dans B
cond3 <- vrai xor faux

AFFICHER("cond1 = ") AFFICHER_LIGNE(cond1)
AFFICHER("cond2 = ") AFFICHER_LIGNE(cond2)
AFFICHER("cond3 = ") AFFICHER_LIGNE(cond3)


# ---------------------------------------------------------------------
#   4) SI / SINON SI / SINON
#   --------------------------------------------------------------------- */

afficher_titre("Test SI / SINON SI / SINON")

SOIT note dans Z tel que note <- 14

SI note >= 16 ALORS
    AFFICHER_LIGNE("Mention Tres Bien")
SINON SI note >= 14 ALORS
    AFFICHER_LIGNE("Mention Bien")
SINON SI note >= 12 ALORS
    AFFICHER_LIGNE("Mention Assez Bien")
SINON
    AFFICHER_LIGNE("Pas de mention")
FIN


# ---------------------------------------------------------------------
#   5) TANT QUE + SORTIR + CONTINUER
#   -------------------------------------------------------------------

afficher_titre("Test TANT QUE avec SORTIR/CONTINUER")

SOIT i dans Z tel que i <- 0
TANT QUE i < 20 FAIRE
    i <- i + 1
    SI (i mod 2) = 0 ALORS
        CONTINUER
    FIN
    SI i > 15 ALORS
        SORTIR
    FIN
    AFFICHER("impair retenu : ") AFFICHER_LIGNE(i)
FIN


#  ---------------------------------------------------------------------
#   6) POUR ... DE ... A ... FAIRE  (avec et sans PAR)
#   --------------------------------------------------------------------- */

afficher_titre("Test POUR SImple")

POUR k DE 1 A 5 FAIRE
    AFFICHER("k = ") AFFICHER_LIGNE(k)
FIN

afficher_titre("Test POUR avec PAR (pas de 2)")

POUR k DE 0 A 10 PAR 2 FAIRE
    AFFICHER("k (pas 2) = ") AFFICHER_LIGNE(k)
FIN


#  ---------------------------------------------------------------------
#   7) REPETER ... JUSQUA
#   --------------------------------------------------------------------- */

afficher_titre("Test REPETER / JUSQUA")

SOIT j dans Z tel que j <- 0
REPETER
    j <- j + 1
    AFFICHER("j = ") AFFICHER_LIGNE(j)
JUSQUA j >= 5


#  ---------------------------------------------------------------------
#   8) FONCTIONS MATHEMATIQUES
#   --------------------------------------------------------------------- */

afficher_titre("Test fonctions mathematiques")

SOIT x dans R tel que x <- 2.0

AFFICHER("sin(x) = ") AFFICHER_LIGNE(sin(x))
AFFICHER("cos(x) = ") AFFICHER_LIGNE(cos(x))
AFFICHER("exp(x) = ") AFFICHER_LIGNE(exp(x))
AFFICHER("log(x) = ") AFFICHER_LIGNE(log(x))
AFFICHER("sqrt(x) = ") AFFICHER_LIGNE(sqrt(x))
AFFICHER("abs(x) = ") AFFICHER_LIGNE(abs(x))
AFFICHER("floor(3.7) = ") AFFICHER_LIGNE(floor(3.7))
AFFICHER("ceil(3.2) = ") AFFICHER_LIGNE(ceil(3.2))
AFFICHER("round(3.5) = ") AFFICHER_LIGNE(round(3.5))


#  ---------------------------------------------------------------------
#   9) NOMBRES COMPLEXES
#   --------------------------------------------------------------------- */

afficher_titre("Test nombres complexes")

SOIT c1 dans C tel que c1 <- 3i
SOIT c2 dans C tel que c2 <- 2.5i

SOIT c_somme dans C
c_somme <- c1 + c2
SOIT c_produit dans C
c_produit <- c1 * c2

AFFICHER("im(c1) = ") AFFICHER_LIGNE(im(c1))
AFFICHER("re(c1) = ") AFFICHER_LIGNE(re(c1))
AFFICHER("arg(c1) = ") AFFICHER_LIGNE(arg(c1))
AFFICHER("sqrt(c1) = ") AFFICHER_LIGNE(sqrt(c1))
AFFICHER("abs(c1) = ") AFFICHER_LIGNE(abs(c1))


#  ---------------------------------------------------------------------
#   10) CHAINES DE CARACTERES
#   --------------------------------------------------------------------

afficher_titre("Test chaines de caracteres")

SOIT nom dans Sigma tel que nom <- "mathlang"
SOIT nom_maj dans Sigma
nom_maj <- majuscules(nom)
SOIT nom_min dans Sigma
nom_min <- minuscules(nom_maj)

AFFICHER_LIGNE(nom_maj)
AFFICHER_LIGNE(nom_min)

SOIT salutation dans Sigma
salutation <- "Bonjour, " + nom
AFFICHER_LIGNE(salutation)


#  ---------------------------------------------------------------------
#   11) LECTURE (LIRE) -- necesSIte une entree utilisateur a l'execution
#   --------------------------------------------------------------------

afficher_titre("Test lecture clavier")

SOIT valeur_lue dans Z
AFFICHER_LIGNE("Entrez un entier :")
LIRE(valeur_lue)
AFFICHER("Vous avez entre : ") AFFICHER_LIGNE(valeur_lue)


#  ---------------------------------------------------------------------
#   12) APPELS DE FONCTIONS / PROCEDURES (y compris recurSIon et appels
#       imbriques)
#  --------------------------------------------------------------------- */

afficher_titre("Test appels de fonctions")

SOIT f5 dans Z
f5 <- factorielle(5)
AFFICHER("factorielle(5) = ") AFFICHER_LIGNE(f5)

SOIT sc dans R
sc <- somme_carres(3.0, 4.0)
AFFICHER("somme_carres(3,4) = ") AFFICHER_LIGNE(sc)

# Appel imbrique : factorielle(factorielle(3) - 5)  => factorielle(1)
SOIT imbrique dans Z
imbrique <- factorielle(factorielle(4) - 5)
AFFICHER("appel imbrique = ") AFFICHER_LIGNE(imbrique)

AFFICHER_LIGNE("=== FIN DES TESTS ===")
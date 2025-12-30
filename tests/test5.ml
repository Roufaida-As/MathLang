# =====================================================
# TEST GÉNÉRATION DE QUADRUPLETS - MATHLANG
# =====================================================

# --- Test 1: Affectations simples ---
SOIT x dans Z tel que x <- 5
SOIT y dans Z tel que y <- 10

# --- Test 2: Addition ---
SOIT z dans Z tel que z <- x + y

# --- Test 3: Soustraction ---
SOIT difference dans Z tel que difference <- x - y

# --- Test 4: Multiplication ---
SOIT prod dans Z tel que prod <- x * y

# --- Test 5: Division réelle ---
SOIT quot_reel dans R tel que quot_reel <- x / y

# --- Test 6: Division entière (div) ---
SOIT quotient dans Z tel que quotient <- (x div y)

# --- Test 7: Modulo (mod) ---
SOIT modulo dans Z tel que modulo <- (x mod y)

# --- Test 8: Puissance ---
SOIT puiss dans R tel que puiss <- (2.0 ^ 3.0)

# --- Test 9: Expression complexe ---
SOIT resultat dans Z tel que resultat <- x + y * 2

# --- Test 10: Expression avec parenthèses ---
SOIT res2 dans Z tel que res2 <- (x + y) * 2

# --- Test 11: Négation ---
SOIT negatif dans Z tel que negatif <- -42

# --- Test 12: sin ---
SOIT s dans R tel que s <- sin(1.57)

# --- Test 13: cos ---
SOIT c dans R tel que c <- cos(0.0)

# --- Test 14: sqrt ---
SOIT racine dans R tel que racine <- sqrt(16.0)

# --- Test 15: exp ---
SOIT expo dans R tel que expo <- exp(1.0)

# --- Test 16: log ---
SOIT logarithme dans R tel que logarithme <- log(2.718)

# --- Test 17: abs ---
SOIT absolu dans R tel que absolu <- abs(-5.5)

# --- Test 18: floor ---
SOIT plancher dans Z tel que plancher <- floor(3.7)

# --- Test 19: ceil ---
SOIT plafond dans Z tel que plafond <- ceil(3.2)

# --- Test 20: round ---
SOIT arrondi dans Z tel que arrondi <- round(3.5)

# --- Test 21: Comparaison < ---
SOIT comp1 dans B tel que comp1 <- x < y

# --- Test 22: Comparaison > ---
SOIT comp2 dans B tel que comp2 <- x > y

# --- Test 23: Comparaison <= ---
SOIT comp3 dans B tel que comp3 <- x <= y

# --- Test 24: Comparaison >= ---
SOIT comp4 dans B tel que comp4 <- x >= y

# --- Test 25: Égalité = ---
SOIT comp5 dans B tel que comp5 <- x = y

# --- Test 26: Différent != ---
SOIT comp6 dans B tel que comp6 <- x != y

# --- Test 27: Booléens vrai/faux ---
SOIT b1 dans B tel que b1 <- vrai
SOIT b2 dans B tel que b2 <- faux

# --- Test 28: ET logique ---
SOIT et_log dans B tel que et_log <- b1 et b2

# --- Test 29: OU logique ---
SOIT ou_log dans B tel que ou_log <- b1 ou b2

# --- Test 30: NON logique ---
SOIT non_log dans B tel que non_log <- non b1

# --- Test 31: XOR logique ---
SOIT xor_log dans B tel que xor_log <- b1 xor b2

# --- Test 32: Expression complexe avec plusieurs opérations ---
SOIT complexe dans R tel que complexe <- (x + y) * (x - y) / 2.0

# --- Test 33: Constante ---
SOIT_CONST PI dans R tel que PI <- 3.14159

# --- Test 34: Expression avec constante ---
SOIT aire dans R tel que aire <- PI * 5.0 * 5.0
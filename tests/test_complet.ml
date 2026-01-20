# ============================================
# Test file for MathLang lexer
# Covers all added tokens
# ============================================

SOIT x dans Z tel que x <- 10
SOIT y dans R tel que y <- 3.14
SOIT z dans C tel que z <- 2.5 - 3i
SOIT c dans C tel que c <- 3.0 + 4.0i

# --- Boolean constants and logic ---
SI vrai et non faux ALORS
    AFFICHER("Boolean logic OK")
FIN

# --- Arithmetic operators ---
SOIT a dans R tel que a <- x + y * 2 - 5 / 3 ^ 2
SOIT b dans Z tel que b <- x div 3
SOIT d dans Z tel que d <- x mod 4

# --- Comparison operators ---
SI x >= 10 et y < 5 ALORS
    AFFICHER("Comparisons OK")
FIN

# --- Control structures ---
POUR i DE 1 A 10 PAR 2 FAIRE
    AFFICHER(i)
FIN

TANT QUE x > 0 FAIRE
    x <- x - 1
FIN

REPETER
    y <- y + 1
JUSQUA y = 10





# --- String functions ---
SOIT msg dans Sigma tel que msg <- "Hello World"
SOIT up dans Sigma tel que up <- majuscules(msg)
SOIT low dans Sigma tel que low <- minuscules(msg)

# --- Matrix functions (commenté - types non encore supportés) ---
# SOIT M dans M tel que M <- identite(3)
# SOIT dM dans R tel que dM <- det(M)
# SOIT iM dans M tel que iM <- inv(M)
# SOIT tM dans R tel que tM <- trace(M)

# --- Set operators (commenté - types non encore supportés) ---
# SOIT A dans E tel que A <- {1, 2, 3}
# SOIT B dans E tel que B <- {3, 4, 5}

# SOIT U dans E tel que U <- A union B
# SOIT I dans E tel que I <- A inter B
# SOIT D dans E tel que D <- A diff B
# SOIT SD dans E tel que SD <- A symdiff B

# SI A inclus U ALORS
#     AFFICHER "Subset OK"
# FIN

# --- Membership (commenté - types non encore supportés) ---
# SI 3 DANS A ALORS
#     AFFICHER "Membership OK"
# FIN

# --- Strings and characters ---
SOIT s dans Sigma tel que s <- "string with \"escape\""
SOIT ch dans Char tel que ch <- 'a'

# --- Lambda / functions (commenté - syntaxe non encore supportée) ---
# SOIT f dans F tel que f <- lambda(x) -> x ^ 2
# SOIT r2 dans R tel que r2 <- f(5)




# ============================================
# Test file for MathLang lexer
# Covers all added tokens
# ============================================

SOIT x <- 10
SOIT y <- 3.14
SOIT z <- 2.5e-3
SOIT c <- 3.0 + 4.0i

# --- Boolean constants and logic ---
SI vrai et non faux ALORS
    AFFICHER_LIGNE "Boolean logic OK"
FIN

# --- Arithmetic operators ---
SOIT a <- x + y * 2 - 5 / 3 ^ 2
SOIT b <- x div 3
SOIT d <- x mod 4

# --- Comparison operators ---
SI x >= 10 et y < 5 ALORS
    AFFICHER "Comparisons OK"
FIN

# --- Control structures ---
POUR i DE 1 A 10 PAR 2 FAIRE
    AFFICHER i
FIN

TANT QUE x > 0 FAIRE
    x <- x - 1
FIN

REPETER
    y <- y + 1
JUSQU'A y = 10

# --- Mathematical functions ---
SOIT s1 <- sin(x)
SOIT s2 <- cos(x)
SOIT s3 <- exp(y)
SOIT s4 <- log(10)
SOIT s5 <- sqrt(16)
SOIT s6 <- abs(-5)
SOIT s7 <- floor(3.7)
SOIT s8 <- ceil(3.1)
SOIT s9 <- round(2.5)

# --- Conversion functions ---
SOIT i1 <- int(3.9)
SOIT r1 <- reel(5)
SOIT b1 <- booleen(vrai)
SOIT ch1 <- chaine(123)

# --- Complex functions ---
SOIT r <- Re(c)
SOIT im <- Im(c)
SOIT ang <- arg(c)

# --- String functions ---
SOIT msg <- "Hello World"
SOIT up <- majuscules(msg)
SOIT low <- minuscules(msg)
SOIT part <- diviser(msg)

# --- Matrix functions ---
SOIT M <- identite(3)
SOIT dM <- det(M)
SOIT iM <- inv(M)
SOIT tM <- trace(M)

# --- Set operators ---
SOIT A <- {1, 2, 3}
SOIT B <- {3, 4, 5}

SOIT U <- A union B
SOIT I <- A inter B
SOIT D <- A diff B
SOIT SD <- A symdiff B

SI A inclus U ALORS
    AFFICHER "Subset OK"
FIN

# --- Membership ---
SI 3 DANS A ALORS
    AFFICHER "Membership OK"
FIN

# --- Strings and characters ---
SOIT s <- "string with \"escape\""
SOIT ch <- 'a'

# --- Lambda / functions ---
SOIT f <- lambda(x) -> x ^ 2
SOIT r2 <- f(5)

# --- End of test ---
AFFICHER_LIGNE "All token tests completed"

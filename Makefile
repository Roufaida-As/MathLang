# ============================================
# Makefile pour MathLang
# ============================================

CC = gcc
CFLAGS = -Wall -g
LEX = flex
YACC = bison

# Fichiers sources
LEX_FILE = mathlang.l
YACC_FILE = mathlang.y
TOKENS_H = tokens.h

# Fichiers générés
LEX_C = lex.yy.c
YACC_C = mathlang.tab.c
YACC_H = mathlang.tab.h

# Exécutables
LEXER_TEST = lexer_test
PARSER = parser

# ============================================
# Règles principales
# ============================================

all: $(PARSER)

# Compilation du parser complet
$(PARSER): $(YACC_C) $(LEX_C)
        $(CC) $(CFLAGS) -o $(PARSER) $(YACC_C) $(LEX_C) -lfl

# Génération du parser Bison
$(YACC_C) $(YACC_H): $(YACC_FILE) $(TOKENS_H)
        $(YACC) -d -v $(YACC_FILE)

# Génération du lexer Flex
$(LEX_C): $(LEX_FILE) $(TOKENS_H)
        $(LEX) $(LEX_FILE)

# Test du lexer uniquement
lexer: $(LEX_C) lexer_test.c $(TOKENS_H)
        $(CC) $(CFLAGS) -o $(LEXER_TEST) lexer_test.c $(LEX_C) -lfl

# ============================================
# Tests
# ============================================

test-lexer: lexer
        @echo "=== Test du Lexer ==="
        @if [ -f test_lexer.math ]; then \
           ./$(LEXER_TEST) test_lexer.math; \
        else \
           echo "Fichier test_lexer.math non trouvé"; \
        fi

test-parser: $(PARSER)
        @echo "=== Test du Parser ==="
        @if [ -f test_parser.ml ]; then \
           ./$(PARSER) test_parser.ml; \
        else \
           echo "Fichier test_parser.math non trouvé"; \
        fi

# Test interactif
interactive: $(PARSER)
        ./$(PARSER)

# ============================================
# Nettoyage
# ============================================

clean:
        rm -f $(LEX_C) $(YACC_C) $(YACC_H) $(LEXER_TEST) $(PARSER)
        rm -f *.o
        rm -f mathlang.output

clean-all: clean
        rm -f *~

# ============================================
# Aide
# ============================================

help:
        @echo "Commandes disponibles:"
        @echo "  make              - Compile le parser"
        @echo "  make lexer        - Compile le test du lexer"
        @echo "  make test-lexer   - Teste le lexer"
        @echo "  make test-parser  - Teste le parser"
        @echo "  make interactive  - Lance le parser en mode interactif"
        @echo "  make clean        - Supprime les fichiers générés"
        @echo "  make clean-all    - Nettoyage complet"

.PHONY: all lexer test-lexer test-parser interactive clean clean-all help
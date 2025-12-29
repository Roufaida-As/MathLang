/* ============================================ */
/* TEST DE LA TABLE DES SYMBOLES - MATHLANG     */
/* ============================================ */

#include "symbol_table.h"
#include <stdio.h>
#include <stdbool.h>

/* Couleurs pour l'affichage */
#define COLOR_GREEN  "\033[0;32m"
#define COLOR_RED    "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE   "\033[0;34m"
#define COLOR_RESET  "\033[0m"

static int tests_passed = 0;
static int tests_failed = 0;

/* ========================================================= */
/* OUTILS DE TEST                                            */
/* ========================================================= */

void print_test_header(const char* test_name) {
    printf("\n" COLOR_BLUE
           "═══════════════════════════════════════════════════════════\n"
           "  TEST: %s\n"
           "═══════════════════════════════════════════════════════════"
           COLOR_RESET "\n",
           test_name);
}

void assert_test(const char* description, bool condition) {
    if (condition) {
        printf(COLOR_GREEN "✓ PASS" COLOR_RESET ": %s\n", description);
        tests_passed++;
    } else {
        printf(COLOR_RED "✗ FAIL" COLOR_RESET ": %s\n", description);
        tests_failed++;
    }
}

/* ========================================================= */
/* AFFICHAGE TABLE                                           */
/* ========================================================= */

static void show_table(const char* title, SymbolTable* table) {
    printf("\n" COLOR_YELLOW ">>> SYMBOL TABLE AFTER TEST: %s <<<\n" COLOR_RESET, title);
    print_symbol_table(table);
}

/* ========================================================= */
/* TEST 1 : INITIALISATION                                   */
/* ========================================================= */

void test_initialization(void) {
    print_test_header("Initialisation");

    SymbolTable* table = init_symbol_table();

    assert_test("Table non NULL", table != NULL);
    assert_test("Table vide", table->count == 0);
    assert_test("Portée initiale = 0", table->current_scope == 0);

    show_table("Initialisation", table);
    free_symbol_table(table);
}

/* ========================================================= */
/* TEST 2 : AJOUT DE SYMBOLES                                 */
/* ========================================================= */

void test_add_symbols(void) {
    print_test_header("Ajout de symboles");

    SymbolTable* table = init_symbol_table();

    bool a = add_symbol(table, "x", SYMBOL_VARIABLE, TYPE_Z, SUBTYPE_DEFAULT, 1, 1);
    bool b = add_symbol(table, "y", SYMBOL_VARIABLE, TYPE_R, SUBTYPE_DEFAULT, 2, 1);
    bool c = add_symbol(table, "flag", SYMBOL_VARIABLE, TYPE_B, SUBTYPE_DEFAULT, 3, 1);

    assert_test("Ajout x", a);
    assert_test("Ajout y", b);
    assert_test("Ajout flag", c);
    assert_test("Compteur = 3", table->count == 3);

    bool dup = add_symbol(table, "x", SYMBOL_VARIABLE, TYPE_Z, SUBTYPE_DEFAULT, 4, 1);
    assert_test("Redéclaration refusée", !dup);

    show_table("Ajout de symboles", table);
    free_symbol_table(table);
}

/* ========================================================= */
/* TEST 3 : RECHERCHE                                        */
/* ========================================================= */

void test_find_symbols(void) {
    print_test_header("Recherche de symboles");

    SymbolTable* table = init_symbol_table();

    add_symbol(table, "a", SYMBOL_VARIABLE, TYPE_Z, SUBTYPE_DEFAULT, 1, 1);
    add_symbol(table, "pi", SYMBOL_CONSTANT, TYPE_R, SUBTYPE_DEFAULT, 2, 1);

    SymbolEntry* a = find_symbol(table, "a");
    SymbolEntry* pi = find_symbol(table, "pi");
    SymbolEntry* nope = find_symbol(table, "nope");

    assert_test("'a' trouvé", a != NULL);
    assert_test("'a' type Z", a && a->type == TYPE_Z);
    assert_test("'pi' constante", pi && pi->is_const);
    assert_test("'nope' absent", nope == NULL);

    show_table("Recherche de symboles", table);
    free_symbol_table(table);
}

/* ========================================================= */
/* TEST 4 : PORTÉES                                          */
/* ========================================================= */

void test_scopes(void) {
    print_test_header("Gestion des portées");

    SymbolTable* table = init_symbol_table();

    add_symbol(table, "x", SYMBOL_VARIABLE, TYPE_Z, SUBTYPE_DEFAULT, 1, 1);
    show_table("Après déclaration globale", table);

    enter_scope(table);
    add_symbol(table, "x", SYMBOL_VARIABLE, TYPE_R, SUBTYPE_DEFAULT, 2, 1);
    show_table("Après masquage (scope 1)", table);

    SymbolEntry* x = find_symbol(table, "x");
    assert_test("Masquage x -> R", x && x->type == TYPE_R);

    exit_scope(table);
    show_table("Après sortie de portée", table);

    x = find_symbol(table, "x");
    assert_test("Retour x -> Z", x && x->type == TYPE_Z);

    free_symbol_table(table);
}

/* ========================================================= */
/* TEST 5 : ÉTAT DES SYMBOLES                                 */
/* ========================================================= */

void test_symbol_state(void) {
    print_test_header("État des symboles");

    SymbolTable* table = init_symbol_table();
    add_symbol(table, "v", SYMBOL_VARIABLE, TYPE_Z, SUBTYPE_DEFAULT, 1, 1);

    SymbolEntry* v = find_symbol(table, "v");

    assert_test("Non initialisée", !v->is_initialized);
    assert_test("Non utilisée", !v->is_used);

    mark_symbol_initialized(v);
    mark_symbol_used(v);

    assert_test("Initialisée", v->is_initialized);
    assert_test("Utilisée", v->is_used);

    show_table("État des symboles", table);
    free_symbol_table(table);
}

/* ========================================================= */
/* TEST 6 : COMPATIBILITÉ DE TYPES                            */
/* ========================================================= */

void test_type_compatibility(void) {
    print_test_header("Compatibilité de types");

    assert_test("Z -> Z", check_type_compatibility(TYPE_Z, TYPE_Z));
    assert_test("Z -> R", check_type_compatibility(TYPE_R, TYPE_Z));
    assert_test("R -> C", check_type_compatibility(TYPE_C, TYPE_R));
    assert_test("R -> Z refusé", !check_type_compatibility(TYPE_Z, TYPE_R));
    assert_test("B -> Z refusé", !check_type_compatibility(TYPE_Z, TYPE_B));
}

/* ========================================================= */
/* TEST 7 : INFÉRENCE DE TYPES                                */
/* ========================================================= */

void test_type_inference(void) {
    print_test_header("Inférence de types");

    assert_test("Z + Z = Z",
        infer_binary_operation_type(TYPE_Z, TYPE_Z, OP_ADD) == TYPE_Z);

    assert_test("Z + R = R",
        infer_binary_operation_type(TYPE_Z, TYPE_R, OP_ADD) == TYPE_R);

    assert_test("R * C = C",
        infer_binary_operation_type(TYPE_R, TYPE_C, OP_MUL) == TYPE_C);

    assert_test("Z < Z = B",
        infer_binary_operation_type(TYPE_Z, TYPE_Z, OP_LT) == TYPE_B);

    assert_test("B AND B = B",
        infer_binary_operation_type(TYPE_B, TYPE_B, OP_AND) == TYPE_B);

    assert_test("B + Z = ERROR",
        infer_binary_operation_type(TYPE_B, TYPE_Z, OP_ADD) == TYPE_ERROR);
}

/* ========================================================= */
/* TEST 8 : PRÉDICATS DE TYPES                                */
/* ========================================================= */

void test_type_predicates(void) {
    print_test_header("Prédicats de types");

    assert_test("Z numérique", is_numeric_type(TYPE_Z));
    assert_test("R numérique", is_numeric_type(TYPE_R));
    assert_test("B non numérique", !is_numeric_type(TYPE_B));

    assert_test("Z ordonnable", is_orderable_type(TYPE_Z));
    assert_test("Σ ordonnable", is_orderable_type(TYPE_SIGMA));
    assert_test("C non ordonnable", !is_orderable_type(TYPE_C));
}

/* ========================================================= */
/* TEST 9 : SCÉNARIO RÉALISTE                                 */
/* ========================================================= */

void test_realistic_scenario(void) {
    print_test_header("Scénario réaliste");

    SymbolTable* table = init_symbol_table();

    add_symbol(table, "x", SYMBOL_VARIABLE, TYPE_Z, SUBTYPE_DEFAULT, 1, 1);
    add_symbol(table, "pi", SYMBOL_CONSTANT, TYPE_R, SUBTYPE_DEFAULT, 2, 1);

    SymbolEntry* x = find_symbol(table, "x");
    SymbolEntry* pi = find_symbol(table, "pi");

    mark_symbol_initialized(x);
    mark_symbol_used(x);
    mark_symbol_used(pi);

    enter_scope(table);
    add_symbol(table, "temp", SYMBOL_VARIABLE, TYPE_R, SUBTYPE_DEFAULT, 3, 5);

    SymbolEntry* temp = find_symbol(table, "temp");
    mark_symbol_initialized(temp);
    mark_symbol_used(temp);

    assert_test("x accessible dans bloc", find_symbol(table, "x") != NULL);
    assert_test("temp accessible dans bloc", temp != NULL);

    show_table("Dans le bloc", table);

    exit_scope(table);

    assert_test("temp supprimé", find_symbol(table, "temp") == NULL);

    show_table("Après sortie du bloc", table);
    free_symbol_table(table);
}

/* ========================================================= */
/* MAIN                                                      */
/* ========================================================= */

int main(void) {
    set_semantic_error_mode(SEMANTIC_NON_FATAL);

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║  SUITE DE TESTS - TABLE DES SYMBOLES MATHLANG              ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");

    test_initialization();
    test_add_symbols();
    test_find_symbols();
    test_scopes();
    test_symbol_state();
    test_type_compatibility();
    test_type_inference();
    test_type_predicates();
    test_realistic_scenario();

    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║ RÉSULTATS FINAUX                                          ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");
    printf("║ ✓ Tests réussis : %-3d                                   ║\n", tests_passed);
    printf("║ ✗ Tests échoués : %-3d                                   ║\n", tests_failed);
    printf("╚═══════════════════════════════════════════════════════════╝\n");

    return tests_failed == 0 ? 0 : 1;
}

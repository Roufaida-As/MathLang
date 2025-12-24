#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STATES 50
#define MAX_TRANSITIONS 100
#define MAX_SYMBOLS 128

// Structure pour une transition
typedef struct {
    int from_state;
    int to_state;
    char symbol;  // '\0' pour epsilon
} Transition;

// Structure pour un automate
typedef struct {
    int num_states;
    int initial_state;
    bool accepting_states[MAX_STATES];
    Transition transitions[MAX_TRANSITIONS];
    int num_transitions;
    char alphabet[MAX_SYMBOLS];
    int alphabet_size;
} Automaton;

// Structure pour un ensemble d'états (pour la construction du AFD)
typedef struct {
    int states[MAX_STATES];
    int size;
} StateSet;

// Fonction pour initialiser un automate
void init_automaton(Automaton *aut) {
    aut->num_states = 0;
    aut->initial_state = 0;
    aut->num_transitions = 0;
    aut->alphabet_size = 0;
    for (int i = 0; i < MAX_STATES; i++) {
        aut->accepting_states[i] = false;
    }
}

// Ajouter une transition
void add_transition(Automaton *aut, int from, int to, char symbol) {
    if (aut->num_transitions < MAX_TRANSITIONS) {
        aut->transitions[aut->num_transitions].from_state = from;
        aut->transitions[aut->num_transitions].to_state = to;
        aut->transitions[aut->num_transitions].symbol = symbol;
        aut->num_transitions++;
    }
}

// Ajouter un symbole à l'alphabet
void add_to_alphabet(Automaton *aut, char symbol) {
    if (symbol == '\0') return; // Ignore epsilon
    for (int i = 0; i < aut->alphabet_size; i++) {
        if (aut->alphabet[i] == symbol) return; // Déjà présent
    }
    aut->alphabet[aut->alphabet_size++] = symbol;
}

// Afficher un automate
void print_automaton(Automaton *aut, const char *name) {
    printf("\n=== %s ===\n", name);
    printf("Nombre d'états: %d\n", aut->num_states);
    printf("État initial: q%d\n", aut->initial_state);
    
    printf("États acceptants: ");
    for (int i = 0; i < aut->num_states; i++) {
        if (aut->accepting_states[i]) {
            printf("q%d ", i);
        }
    }
    printf("\n");
    
    printf("Transitions:\n");
    for (int i = 0; i < aut->num_transitions; i++) {
        if (aut->transitions[i].symbol == '\0') {
            printf("  q%d --ε--> q%d\n", 
                   aut->transitions[i].from_state,
                   aut->transitions[i].to_state);
        } else {
            printf("  q%d --%c--> q%d\n", 
                   aut->transitions[i].from_state,
                   aut->transitions[i].symbol,
                   aut->transitions[i].to_state);
        }
    }
}

// Calculer l'epsilon-fermeture d'un état
void epsilon_closure(Automaton *aut, int state, bool *visited) {
    visited[state] = true;
    
    for (int i = 0; i < aut->num_transitions; i++) {
        if (aut->transitions[i].from_state == state && 
            aut->transitions[i].symbol == '\0' && 
            !visited[aut->transitions[i].to_state]) {
            epsilon_closure(aut, aut->transitions[i].to_state, visited);
        }
    }
}

// Calculer l'epsilon-fermeture d'un ensemble d'états
void epsilon_closure_set(Automaton *aut, StateSet *set, StateSet *result) {
    bool visited[MAX_STATES] = {false};
    
    for (int i = 0; i < set->size; i++) {
        epsilon_closure(aut, set->states[i], visited);
    }
    
    result->size = 0;
    for (int i = 0; i < aut->num_states; i++) {
        if (visited[i]) {
            result->states[result->size++] = i;
        }
    }
}

// Calculer la transition d'un ensemble d'états avec un symbole
void move(Automaton *aut, StateSet *set, char symbol, StateSet *result) {
    result->size = 0;
    bool added[MAX_STATES] = {false};
    
    for (int i = 0; i < set->size; i++) {
        int state = set->states[i];
        for (int j = 0; j < aut->num_transitions; j++) {
            if (aut->transitions[j].from_state == state && 
                aut->transitions[j].symbol == symbol &&
                !added[aut->transitions[j].to_state]) {
                result->states[result->size++] = aut->transitions[j].to_state;
                added[aut->transitions[j].to_state] = true;
            }
        }
    }
}

// Comparer deux ensembles d'états
bool state_sets_equal(StateSet *s1, StateSet *s2) {
    if (s1->size != s2->size) return false;
    
    for (int i = 0; i < s1->size; i++) {
        bool found = false;
        for (int j = 0; j < s2->size; j++) {
            if (s1->states[i] == s2->states[j]) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

// Convertir AFN en AFD (construction des sous-ensembles)
void nfa_to_dfa(Automaton *nfa, Automaton *dfa) {
    init_automaton(dfa);
    
    StateSet dfa_states[MAX_STATES];
    int num_dfa_states = 0;
    
    // État initial du DFA = epsilon-fermeture de l'état initial du NFA
    StateSet initial_set;
    initial_set.size = 1;
    initial_set.states[0] = nfa->initial_state;
    
    epsilon_closure_set(nfa, &initial_set, &dfa_states[0]);
    num_dfa_states = 1;
    dfa->initial_state = 0;
    
    // Copier l'alphabet
    memcpy(dfa->alphabet, nfa->alphabet, nfa->alphabet_size);
    dfa->alphabet_size = nfa->alphabet_size;
    
    // Construction des états et transitions du DFA
    int current = 0;
    while (current < num_dfa_states) {
        StateSet *current_set = &dfa_states[current];
        
        // Vérifier si cet ensemble contient un état acceptant
        for (int i = 0; i < current_set->size; i++) {
            if (nfa->accepting_states[current_set->states[i]]) {
                dfa->accepting_states[current] = true;
                break;
            }
        }
        
        // Pour chaque symbole de l'alphabet
        for (int sym = 0; sym < nfa->alphabet_size; sym++) {
            char symbol = nfa->alphabet[sym];
            
            StateSet move_result, closure_result;
            move(nfa, current_set, symbol, &move_result);
            
            if (move_result.size == 0) continue;
            
            epsilon_closure_set(nfa, &move_result, &closure_result);
            
            // Chercher si cet ensemble existe déjà
            int target_state = -1;
            for (int i = 0; i < num_dfa_states; i++) {
                if (state_sets_equal(&closure_result, &dfa_states[i])) {
                    target_state = i;
                    break;
                }
            }
            
            // Si l'ensemble n'existe pas, le créer
            if (target_state == -1) {
                target_state = num_dfa_states;
                dfa_states[num_dfa_states] = closure_result;
                num_dfa_states++;
            }
            
            // Ajouter la transition
            add_transition(dfa, current, target_state, symbol);
        }
        
        current++;
    }
    
    dfa->num_states = num_dfa_states;
}

// ==================== Construction d'AFN pour différentes expressions ====================

// AFN pour Identificateur: [a-zA-Z_][a-zA-Z0-9_]*
void build_identifier_nfa(Automaton *aut) {
    init_automaton(aut);
    aut->num_states = 6;
    aut->initial_state = 0;
    aut->accepting_states[2] = true;
    aut->accepting_states[5] = true;
    
    // Construction selon Thompson
    // q0 --[a-zA-Z_]--> q1
    for (char c = 'a'; c <= 'z'; c++) {
        add_transition(aut, 0, 1, c);
        add_to_alphabet(aut, c);
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        add_transition(aut, 0, 1, c);
        add_to_alphabet(aut, c);
    }
    add_transition(aut, 0, 1, '_');
    add_to_alphabet(aut, '_');
    
    // q1 --epsilon--> q2 (acceptant - identificateur d'un seul caractère valide)
    add_transition(aut, 1, 2, '\0');
    
    // q2 --epsilon--> q5 (peut accepter directement)
    add_transition(aut, 2, 5, '\0');
    
    // q2 --epsilon--> q3 (pour continuer la boucle de Kleene)
    add_transition(aut, 2, 3, '\0');
    
    // q3 --[a-zA-Z0-9_]--> q4
    for (char c = 'a'; c <= 'z'; c++) {
        add_transition(aut, 3, 4, c);
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        add_transition(aut, 3, 4, c);
    }
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 3, 4, c);
        add_to_alphabet(aut, c);
    }
    add_transition(aut, 3, 4, '_');
    
    // q4 --epsilon--> q3 (boucle pour répéter)
    add_transition(aut, 4, 3, '\0');
    
    // q4 --epsilon--> q5 (état final)
    add_transition(aut, 4, 5, '\0');
}

// AFN pour Entier: [0-9]+
void build_integer_nfa(Automaton *aut) {
    init_automaton(aut);
    aut->num_states = 5;
    aut->initial_state = 0;
    aut->accepting_states[4] = true;
    
    // q0 --[0-9]--> q1
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 0, 1, c);
        add_to_alphabet(aut, c);
    }
    
    // q1 --epsilon--> q2
    add_transition(aut, 1, 2, '\0');
    
    // q1 --epsilon--> q4
    add_transition(aut, 1, 4, '\0');
    
    // q2 --[0-9]--> q3
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 2, 3, c);
    }
    
    // q3 --epsilon--> q4
    add_transition(aut, 3, 4, '\0');
    
    // q3 --epsilon--> q2 (boucle)
    add_transition(aut, 3, 2, '\0');
}

// AFN pour mot-clé "SOIT"
void build_soit_nfa(Automaton *aut) {
    init_automaton(aut);
    aut->num_states = 5;
    aut->initial_state = 0;
    aut->accepting_states[4] = true;
    
    add_transition(aut, 0, 1, 'S');
    add_transition(aut, 1, 2, 'O');
    add_transition(aut, 2, 3, 'I');
    add_transition(aut, 3, 4, 'T');
    
    add_to_alphabet(aut, 'S');
    add_to_alphabet(aut, 'O');
    add_to_alphabet(aut, 'I');
    add_to_alphabet(aut, 'T');
}

// AFN pour mot-clé "dans"
void build_dans_nfa(Automaton *aut) {
    init_automaton(aut);
    aut->num_states = 5;
    aut->initial_state = 0;
    aut->accepting_states[4] = true;
    
    add_transition(aut, 0, 1, 'd');
    add_transition(aut, 1, 2, 'a');
    add_transition(aut, 2, 3, 'n');
    add_transition(aut, 3, 4, 's');
    
    add_to_alphabet(aut, 'd');
    add_to_alphabet(aut, 'a');
    add_to_alphabet(aut, 'n');
    add_to_alphabet(aut, 's');
}

// AFN pour Nombre réel: [0-9]+\.[0-9]+
void build_real_nfa(Automaton *aut) {
    init_automaton(aut);
    aut->num_states = 4;
    aut->initial_state = 0;
    aut->accepting_states[3] = true;
    
    // q0 --[0-9]--> q1
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 0, 1, c);
        add_to_alphabet(aut, c);
    }
    
    // q1 --[0-9]--> q1 (boucle)
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 1, 1, c);
    }
    
    // q1 --.--> q2
    add_transition(aut, 1, 2, '.');
    add_to_alphabet(aut, '.');
    
    // q2 --[0-9]--> q3
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 2, 3, c);
    }
    
    // q3 --[0-9]--> q3 (boucle)
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 3, 3, c);
    }
}

// AFN pour Nombre complexe: [0-9]+(\.[0-9]+)?(+|-)[0-9]+(\.[0-9]+)?i
void build_complex_nfa(Automaton *aut) {
    init_automaton(aut);
    aut->num_states = 17;
    aut->initial_state = 0;
    aut->accepting_states[16] = true;
    
    // Partie réelle: [0-9]+
    // q0 --[0-9]--> q1
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 0, 1, c);
        add_to_alphabet(aut, c);
    }
    
    // q1 --[0-9]--> q1 (boucle)
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 1, 1, c);
    }
    
    // Partie décimale optionnelle: (\.[0-9]+)?
    // q1 --epsilon--> q2
    add_transition(aut, 1, 2, '\0');
    
    // q2 --.--> q3
    add_transition(aut, 2, 3, '.');
    add_to_alphabet(aut, '.');
    
    // q3 --[0-9]--> q4
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 3, 4, c);
    }
    
    // q4 --[0-9]--> q4 (boucle)
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 4, 4, c);
    }
    
    // q4 --epsilon--> q5
    add_transition(aut, 4, 5, '\0');
    
    // q1 --epsilon--> q5 (skip décimale)
    add_transition(aut, 1, 5, '\0');
    
    // Opérateur +|-
    // q5 --epsilon--> q6 (pour -)
    add_transition(aut, 5, 6, '\0');
    
    // q6 ----> q7
    add_transition(aut, 6, 7, '-');
    add_to_alphabet(aut, '-');
    
    // q5 --epsilon--> q8 (pour +)
    add_transition(aut, 5, 8, '\0');
    
    // q8 --+--> q9
    add_transition(aut, 8, 9, '+');
    add_to_alphabet(aut, '+');
    
    // q7 --epsilon--> q10
    add_transition(aut, 7, 10, '\0');
    
    // q9 --epsilon--> q10
    add_transition(aut, 9, 10, '\0');
    
    // Partie imaginaire: [0-9]+
    // q10 --[0-9]--> q11
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 10, 11, c);
    }
    
    // q11 --[0-9]--> q11 (boucle)
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 11, 11, c);
    }
    
    // Partie décimale optionnelle: (\.[0-9]+)?
    // q11 --epsilon--> q12
    add_transition(aut, 11, 12, '\0');
    
    // q12 --.--> q13
    add_transition(aut, 12, 13, '.');
    
    // q13 --[0-9]--> q14
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 13, 14, c);
    }
    
    // q14 --[0-9]--> q14 (boucle)
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 14, 14, c);
    }
    
    // q14 --epsilon--> q15
    add_transition(aut, 14, 15, '\0');
    
    // q11 --epsilon--> q15 (skip décimale)
    add_transition(aut, 11, 15, '\0');
    
    // q15 --i--> q16
    add_transition(aut, 15, 16, 'i');
    add_to_alphabet(aut, 'i');
}

// AFN pour affectation "<-"
void build_affectation_nfa(Automaton *aut) {
    init_automaton(aut);
    aut->num_states = 3;
    aut->initial_state = 0;
    aut->accepting_states[2] = true;
    
    add_transition(aut, 0, 1, '<');
    add_transition(aut, 1, 2, '-');
    
    add_to_alphabet(aut, '<');
    add_to_alphabet(aut, '-');
}

// ==================== Programme Principal ====================

int main() {
    Automaton nfa, dfa;
    
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║   Construction AFN et AFD - Analyse Lexicale Manuelle          ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    
    // Test 1: Identificateur (CORRIGÉ)
    printf("\n\n┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ 1. IDENTIFICATEUR: [a-zA-Z_][a-zA-Z0-9_]*                       │\n");
    printf("└─────────────────────────────────────────────────────────────────┘\n");
    
    build_identifier_nfa(&nfa);
    print_automaton(&nfa, "AFN - Identificateur");
    
    nfa_to_dfa(&nfa, &dfa);
    print_automaton(&dfa, "AFD - Identificateur");
    
    // Test 2: Mot-clé SOIT
    printf("\n\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ 2. MOT-CLÉ: SOIT                                            │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    build_soit_nfa(&nfa);
    print_automaton(&nfa, "AFN/AFD - SOIT (déjà déterministe)");
    
    // Test 3: Mot-clé dans
    printf("\n\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ 3. MOT-CLÉ: dans                                            │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    build_dans_nfa(&nfa);
    print_automaton(&nfa, "AFN/AFD - dans (déjà déterministe)");
    
    // Test 4: Entier
    printf("\n\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ 4. ENTIER: [0-9]+                                           │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    build_integer_nfa(&nfa);
    print_automaton(&nfa, "AFN - Entier");
    
    nfa_to_dfa(&nfa, &dfa);
    print_automaton(&dfa, "AFD - Entier");
    
    // Test 5: Nombre réel
    printf("\n\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ 5. NOMBRE RÉEL: [0-9]+\\.[0-9]+                              │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    build_real_nfa(&nfa);
    print_automaton(&nfa, "AFN/AFD - Nombre réel (déjà déterministe)");
    
    // Test 6: Nombre complexe
    printf("\n\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ 6. NOMBRE COMPLEXE: [0-9]+(\\.[0-9]+)?(+|-)[0-9]+(\\.[0-9]+)?i│\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    build_complex_nfa(&nfa);
    print_automaton(&nfa, "AFN - Nombre complexe");
    
    nfa_to_dfa(&nfa, &dfa);
    print_automaton(&dfa, "AFD - Nombre complexe");
    
    // Test 7: Affectation
    printf("\n\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│ 7. AFFECTATION: <-                                          │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n");
    
    build_affectation_nfa(&nfa);
    print_automaton(&nfa, "AFN/AFD - Affectation (déjà déterministe)");
    
    printf("\n\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║   Résumé de l'analyse                                          ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    printf("║   Tokens implémentés: 7                                        ║\n");
    printf("║   - 1 Identificateur [a-zA-Z_][a-zA-Z0-9_]*                    ║\n");
    printf("║   - 2 Mots-clés (SOIT, dans)                                   ║\n");
    printf("║   - 3 Types numériques (Entier, Réel, Complexe)                ║\n");
    printf("║   - 1 Opérateur (Affectation)                                  ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
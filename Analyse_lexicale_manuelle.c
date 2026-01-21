#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STATES 50
#define MAX_TRANSITIONS 500
#define MAX_SYMBOLS 128
#define MAX_INPUT 256

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

// ==================== Fonctions de reconnaissance ====================

// Simuler un AFN sur une chaîne d'entrée
bool simulate_nfa(Automaton *nfa, const char *input) {
    StateSet current_states, next_states;
    
    // État initial avec epsilon-fermeture
    current_states.size = 1;
    current_states.states[0] = nfa->initial_state;
    epsilon_closure_set(nfa, &current_states, &current_states);
    
    // Traiter chaque caractère
    for (int i = 0; input[i] != '\0'; i++) {
        move(nfa, &current_states, input[i], &next_states);
        
        if (next_states.size == 0) {
            return false; // Aucune transition possible
        }
        
        epsilon_closure_set(nfa, &next_states, &current_states);
    }
    
    // Vérifier si un état final est atteint
    for (int i = 0; i < current_states.size; i++) {
        if (nfa->accepting_states[current_states.states[i]]) {
            return true;
        }
    }
    
    return false;
}


// ==================== Construction d'AFN pour différentes expressions ====================

// AFN pour Identificateur: [a-zA-Z_][a-zA-Z0-9_]*
void build_identifier_nfa(Automaton *aut) {
    init_automaton(aut);
    aut->num_states = 2;
    aut->initial_state = 0;
    aut->accepting_states[1] = true;
    
    // Premier caractère: [a-zA-Z_] - transition de 0 vers 1
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
    
    // Caractères suivants: [a-zA-Z0-9_]* - boucle sur l'état 1
    for (char c = 'a'; c <= 'z'; c++) {
        add_transition(aut, 1, 1, c);
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        add_transition(aut, 1, 1, c);
    }
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 1, 1, c);
        add_to_alphabet(aut, c);
    }
    add_transition(aut, 1, 1, '_');
    
    // DEBUG
    print_automaton_debug(aut, "IDENTIFICATEUR NFA");
}
// AFN pour Entier: [0-9]+
void build_integer_nfa(Automaton *aut) {
    init_automaton(aut);
    aut->num_states = 5;
    aut->initial_state = 0;
    aut->accepting_states[4] = true;
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 0, 1, c);
        add_to_alphabet(aut, c);
    }
    
    add_transition(aut, 1, 2, '\0');
    add_transition(aut, 1, 4, '\0');
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 2, 3, c);
    }
    
    add_transition(aut, 3, 4, '\0');
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
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 0, 1, c);
        add_to_alphabet(aut, c);
    }
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 1, 1, c);
    }
    
    add_transition(aut, 1, 2, '.');
    add_to_alphabet(aut, '.');
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 2, 3, c);
    }
    
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
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 0, 1, c);
        add_to_alphabet(aut, c);
    }
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 1, 1, c);
    }
    
    add_transition(aut, 1, 2, '\0');
    add_transition(aut, 2, 3, '.');
    add_to_alphabet(aut, '.');
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 3, 4, c);
    }
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 4, 4, c);
    }
    
    add_transition(aut, 4, 5, '\0');
    add_transition(aut, 1, 5, '\0');
    add_transition(aut, 5, 6, '\0');
    add_transition(aut, 6, 7, '-');
    add_to_alphabet(aut, '-');
    add_transition(aut, 5, 8, '\0');
    add_transition(aut, 8, 9, '+');
    add_to_alphabet(aut, '+');
    add_transition(aut, 7, 10, '\0');
    add_transition(aut, 9, 10, '\0');
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 10, 11, c);
    }
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 11, 11, c);
    }
    
    add_transition(aut, 11, 12, '\0');
    add_transition(aut, 12, 13, '.');
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 13, 14, c);
    }
    
    for (char c = '0'; c <= '9'; c++) {
        add_transition(aut, 14, 14, c);
    }
    
    add_transition(aut, 14, 15, '\0');
    add_transition(aut, 11, 15, '\0');
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

// ==================== Analyse de la chaîne ====================

const char* analyze_string(const char *input) {
    Automaton nfa;
    
    // Vérifier d'abord les mots-clés (priorité)
    build_soit_nfa(&nfa);
    if (simulate_nfa(&nfa, input)) {
        return "MOT-CLE (SOIT)";
    }
    
    build_dans_nfa(&nfa);
    if (simulate_nfa(&nfa, input)) {
        return "MOT-CLE (dans)";
    }
    
    // Vérifier l'affectation
    build_affectation_nfa(&nfa);
    if (simulate_nfa(&nfa, input)) {
        return "OPERATEUR (Affectation <-)";
    }
    
    // Vérifier les nombres (du plus spécifique au moins spécifique)
    build_complex_nfa(&nfa);
    if (simulate_nfa(&nfa, input)) {
        return "NOMBRE COMPLEXE";
    }
    
    build_real_nfa(&nfa);
    if (simulate_nfa(&nfa, input)) {
        return "NOMBRE REEL";
    }
    
    build_integer_nfa(&nfa);
    if (simulate_nfa(&nfa, input)) {
        return "ENTIER";
    }
    
    // Vérifier identificateur (en dernier car plus général)
    build_identifier_nfa(&nfa);
    if (simulate_nfa(&nfa, input)) {
        return "IDENTIFICATEUR";
    }
    
    return "TYPE NON EXISTANT";
}

// ==================== Programme Principal ====================

int main() {
    char input[MAX_INPUT];
    
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║         ANALYSEUR LEXICAL INTERACTIF                          ║\n");
    printf("╠════════════════════════════════════════════════════════════════╣\n");
    printf("║  Types reconnus:                                               ║\n");
    printf("║  - IDENTIFICATEUR: [a-zA-Z_][a-zA-Z0-9_]*                      ║\n");
    printf("║  - MOT-CLE: SOIT, dans                                         ║\n");
    printf("║  - ENTIER: [0-9]+                                              ║\n");
    printf("║  - NOMBRE REEL: [0-9]+.[0-9]+                                  ║\n");
    printf("║  - NOMBRE COMPLEXE: [0-9]+(.[0-9]+)?(+|-)[0-9]+(.[0-9]+)?i    ║\n");
    printf("║  - AFFECTATION: <-                                             ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");
    
    while (1) {
        printf("┌────────────────────────────────────────────────────────────────┐\n");
        printf("│ Entrez une chaîne à analyser (ou 'quit' pour quitter):        │\n");
        printf("└────────────────────────────────────────────────────────────────┘\n");
        printf(">>> ");
        
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            break;
        }
        
        // Enlever le saut de ligne
        input[strcspn(input, "\n")] = 0;
        
        // Quitter si demandé
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
            printf("\n👋 Au revoir!\n");
            break;
        }
        
        if (strlen(input) == 0) {
            printf("\n⚠️  Veuillez entrer une chaîne non vide.\n\n");
            continue;
        }
        
        // Analyser la chaîne
        const char *result = analyze_string(input);
        
        printf("\n┌────────────────────────────────────────────────────────────────┐\n");
        printf("│ RÉSULTAT DE L'ANALYSE                                          │\n");
        printf("├────────────────────────────────────────────────────────────────┤\n");
        printf("│ Chaîne: %-54s │\n", input);
        printf("│ Type  : %-54s │\n", result);
        printf("└────────────────────────────────────────────────────────────────┘\n\n");
        
        // Exemples si type non reconnu
        if (strcmp(result, "TYPE NON EXISTANT") == 0) {
            printf("💡 Exemples de chaînes valides:\n");
            printf("   - Identificateur: variable1, _temp, MyVar\n");
            printf("   - Entier: 123, 456\n");
            printf("   - Réel: 3.14, 0.5\n");
            printf("   - Complexe: 3+4i, 2.5-1.2i\n");
            printf("   - Mot-clé: SOIT, dans\n");
            printf("   - Affectation: <-\n\n");
        }
    }
    
    return 0;
}

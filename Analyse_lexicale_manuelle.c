#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN "cls"
#else
#define CLEAR_SCREEN "clear"
#endif

// =========================== STRUCTURES ===========================

// Structure pour un état d'automate
typedef struct State {
    int id;
    bool is_final;
} State;

// Structure pour une transition
typedef struct Transition {
    int from_state;
    int to_state;
    char symbol; // '\0' pour epsilon
} Transition;

// Structure pour un automate
typedef struct Automaton {
    State* states;
    int num_states;
    Transition* transitions;
    int num_transitions;
    int initial_state;
} Automaton;

// =========================== AUTOMATE IDENTIFICATEUR ===========================

// AFN pour identificateur: [a-zA-Z_][a-zA-Z0-9_]*
void create_identifier_nfa(Automaton* nfa) {
    nfa->num_states = 5;
    nfa->states = (State*)malloc(nfa->num_states * sizeof(State));
    
    // États
    nfa->states[0] = (State){0, false}; // initial
    nfa->states[1] = (State){1, false};
    nfa->states[2] = (State){2, true};  // final
    nfa->states[3] = (State){3, false};
    nfa->states[4] = (State){4, false};
    
    nfa->initial_state = 0;
    
    // Transitions
    nfa->num_transitions = 6;
    nfa->transitions = (Transition*)malloc(nfa->num_transitions * sizeof(Transition));
    
    // Première partie: [a-zA-Z_]
    nfa->transitions[0] = (Transition){0, 1, 'L'}; // L = lettre ou _
    nfa->transitions[1] = (Transition){1, 2, '\0'}; // epsilon
    
    // Deuxième partie: [a-zA-Z0-9_]*
    nfa->transitions[2] = (Transition){2, 3, '\0'}; // epsilon vers boucle
    nfa->transitions[3] = (Transition){3, 4, 'A'}; // A = alphanum ou _
    nfa->transitions[4] = (Transition){4, 3, '\0'}; // epsilon retour
    nfa->transitions[5] = (Transition){4, 2, '\0'}; // epsilon vers final
}

// Conversion AFN -> AFD pour identificateur
bool is_identifier_char_first(char c) {
    return isalpha(c) || c == '_';
}

bool is_identifier_char_rest(char c) {
    return isalnum(c) || c == '_';
}

bool recognize_identifier_afd(const char* input) {
    if (!input || strlen(input) == 0) return false;
    
    int state = 0; // S0
    int i = 0;
    
    while (input[i] != '\0') {
        char c = input[i];
        
        switch(state) {
            case 0: // S0
                if (is_identifier_char_first(c)) {
                    state = 1; // SF (état final)
                } else {
                    return false;
                }
                break;
                
            case 1: // SF
                if (is_identifier_char_rest(c)) {
                    state = 1; // reste en SF
                } else {
                    return false;
                }
                break;
                
            default:
                return false;
        }
        i++;
    }
    
    // On accepte si on est dans l'état final (SF = 1)
    return (state == 1);
}

// =========================== AUTOMATE MOT-CLÉ SOIT ===========================

bool recognize_soit(const char* input) {
    return (strcmp(input, "SOIT") == 0);
}

// =========================== AUTOMATE MOT-CLÉ dans ===========================

bool recognize_dans(const char* input) {
    return (strcmp(input, "dans") == 0);
}

// =========================== AUTOMATE ENTIER ===========================

bool recognize_integer_afd(const char* input) {
    if (!input || strlen(input) == 0) return false;
    
    int state = 0; // S0
    int i = 0;
    
    while (input[i] != '\0') {
        char c = input[i];
        
        switch(state) {
            case 0: // S0
                if (isdigit(c)) {
                    state = 1; // SF
                } else {
                    return false;
                }
                break;
                
            case 1: // SF
                if (isdigit(c)) {
                    state = 1; // reste en SF
                } else {
                    return false;
                }
                break;
                
            default:
                return false;
        }
        i++;
    }
    
    return (state == 1);
}

// =========================== AUTOMATE RÉEL ===========================

bool recognize_real_afd(const char* input) {
    if (!input || strlen(input) == 0) return false;
    
    int state = 0; // S0
    int i = 0;
    bool has_dot = false;
    
    while (input[i] != '\0') {
        char c = input[i];
        
        switch(state) {
            case 0: // S0
                if (isdigit(c)) {
                    state = 1;
                } else {
                    return false;
                }
                break;
                
            case 1: // partie entière
                if (isdigit(c)) {
                    state = 1;
                } else if (c == '.' && !has_dot) {
                    has_dot = true;
                    state = 2;
                } else {
                    return false;
                }
                break;
                
            case 2: // après le point
                if (isdigit(c)) {
                    state = 3; // SF
                } else {
                    return false;
                }
                break;
                
            case 3: // SF (partie décimale)
                if (isdigit(c)) {
                    state = 3;
                } else {
                    return false;
                }
                break;
                
            default:
                return false;
        }
        i++;
    }
    
    return (state == 3 && has_dot);
}

// =========================== AUTOMATE COMPLEXE ===========================

bool recognize_complex_afd(const char* input) {
    if (!input || strlen(input) == 0) return false;
    
    int state = 0;
    int i = 0;
    bool has_real_dot = false;
    bool has_imag_dot = false;
    bool has_operator = false;
    
    while (input[i] != '\0') {
        char c = input[i];
        
        switch(state) {
            case 0: // S0 - début
                if (isdigit(c)) {
                    state = 1;
                } else {
                    return false;
                }
                break;
                
            case 1: // partie réelle entière
                if (isdigit(c)) {
                    state = 1;
                } else if (c == '.' && !has_real_dot) {
                    has_real_dot = true;
                    state = 2;
                } else if (c == '+' || c == '-') {
                    has_operator = true;
                    state = 4;
                } else {
                    return false;
                }
                break;
                
            case 2: // après point partie réelle
                if (isdigit(c)) {
                    state = 3;
                } else {
                    return false;
                }
                break;
                
            case 3: // partie réelle décimale
                if (isdigit(c)) {
                    state = 3;
                } else if (c == '+' || c == '-') {
                    has_operator = true;
                    state = 4;
                } else {
                    return false;
                }
                break;
                
            case 4: // après +/-
                if (isdigit(c)) {
                    state = 5;
                } else {
                    return false;
                }
                break;
                
            case 5: // partie imaginaire entière
                if (isdigit(c)) {
                    state = 5;
                } else if (c == '.' && !has_imag_dot) {
                    has_imag_dot = true;
                    state = 6;
                } else if (c == 'i') {
                    state = 8; // SF
                } else {
                    return false;
                }
                break;
                
            case 6: // après point partie imaginaire
                if (isdigit(c)) {
                    state = 7;
                } else {
                    return false;
                }
                break;
                
            case 7: // partie imaginaire décimale
                if (isdigit(c)) {
                    state = 7;
                } else if (c == 'i') {
                    state = 8; // SF
                } else {
                    return false;
                }
                break;
                
            case 8: // SF - après 'i'
                return false; // rien après 'i'
                
            default:
                return false;
        }
        i++;
    }
    
    return (state == 8 && has_operator);
}

// =========================== AUTOMATE AFFECTATION ===========================

bool recognize_assignment(const char* input) {
    return (strcmp(input, "<-") == 0);
}

// =========================== FONCTION PRINCIPALE D'ANALYSE ===========================

typedef enum {
    TYPE_UNKNOWN,
    TYPE_IDENTIFIER,
    TYPE_SOIT,
    TYPE_DANS,
    TYPE_INTEGER,
    TYPE_REAL,
    TYPE_COMPLEX,
    TYPE_ASSIGNMENT
} TokenType;

const char* type_to_string(TokenType type) {
    switch(type) {
        case TYPE_IDENTIFIER: return "IDENTIFICATEUR";
        case TYPE_SOIT: return "MOT-CLE (SOIT)";
        case TYPE_DANS: return "MOT-CLE (dans)";
        case TYPE_INTEGER: return "ENTIER";
        case TYPE_REAL: return "NOMBRE REEL";
        case TYPE_COMPLEX: return "NOMBRE COMPLEXE";
        case TYPE_ASSIGNMENT: return "OPERATEUR (Affectation <-)";
        default: return "TYPE NON EXISTANT";
    }
}

TokenType analyze_string(const char* input) {
    // Ordre de vérification important
    if (recognize_soit(input)) return TYPE_SOIT;
    if (recognize_dans(input)) return TYPE_DANS;
    if (recognize_assignment(input)) return TYPE_ASSIGNMENT;
    if (recognize_complex_afd(input)) return TYPE_COMPLEX;
    if (recognize_real_afd(input)) return TYPE_REAL;
    if (recognize_integer_afd(input)) return TYPE_INTEGER;
    if (recognize_identifier_afd(input)) return TYPE_IDENTIFIER;
    
    return TYPE_UNKNOWN;
}

// =========================== INTERFACE UTILISATEUR ===========================

void print_box_line(const char* text, int width, char border_char) {
    printf("%c ", border_char);
    printf("%-*s", width-2, text);
    printf("%c\n", border_char);
}

void print_header() {
    const int width = 66;
    
    printf("\n");
    for(int i = 0; i < width; i++) printf("=");
    printf("\n");
    print_box_line("         ANALYSEUR LEXICAL INTERACTIF", width, '|');
    for(int i = 0; i < width; i++) printf("=");
    printf("\n");
    print_box_line("  Types reconnus:", width, '|');
    print_box_line("  - IDENTIFICATEUR: [a-zA-Z_][a-zA-Z0-9_]*", width, '|');
    print_box_line("  - MOT-CLE: SOIT, dans", width, '|');
    print_box_line("  - ENTIER: [0-9]+", width, '|');
    print_box_line("  - NOMBRE REEL: [0-9]+.[0-9]+", width, '|');
    print_box_line("  - NOMBRE COMPLEXE: [0-9]+(.[0-9]+)?(+|-)[0-9]+(.[0-9]+)?i", width, '|');
    print_box_line("  - AFFECTATION: <-", width, '|');
    for(int i = 0; i < width; i++) printf("=");
    printf("\n\n");
}

void print_result(const char* input, TokenType type) {
    const int width = 66;
    char buffer[256];
    
    printf("\n");
    for(int i = 0; i < width; i++) printf("-");
    printf("\n");
    print_box_line(" RESULTAT DE L'ANALYSE", width, '|');
    for(int i = 0; i < width; i++) printf("-");
    printf("\n");
    
    snprintf(buffer, sizeof(buffer), " Chaine: %s", input);
    print_box_line(buffer, width, '|');
    
    snprintf(buffer, sizeof(buffer), " Type  : %s", type_to_string(type));
    print_box_line(buffer, width, '|');
    
    for(int i = 0; i < width; i++) printf("-");
    printf("\n");
}

void print_examples() {
    printf("\n Exemples de chaines valides:\n");
    printf("   - Identificateur: variable1, _temp, MyVar\n");
    printf("   - Entier: 123, 456\n");
    printf("   - Reel: 3.14, 0.5\n");
    printf("   - Complexe: 3+4i, 2.5-1.2i\n");
    printf("   - Mot-cle: SOIT, dans\n");
    printf("   - Affectation: <-\n\n");
}

int main() {
    char input[256];
    
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    #endif
    
    print_header();
    
    while(1) {
        for(int i = 0; i < 66; i++) printf("-");
        printf("\n");
        print_box_line(" Entrez une chaine a analyser (ou 'quit' pour quitter):", 66, '|');
        for(int i = 0; i < 66; i++) printf("-");
        printf("\n>>> ");
        
        if (!fgets(input, sizeof(input), stdin)) break;
        
        // Enlever le '\n'
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "quit") == 0) {
            printf("\nAu revoir!\n");
            break;
        }
        
        TokenType type = analyze_string(input);
        print_result(input, type);
        
        if (type == TYPE_UNKNOWN) {
            print_examples();
        }
    }
    
    return 0;
}

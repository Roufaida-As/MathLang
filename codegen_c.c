#include "codegen_c.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ========================================================= */
/*  PETITE TABLE : nom de temporaire -> DataType              */
/* ========================================================= */
/*
 * Vos Quadruplet ne stockent pas le type du résultat, donc on
 * doit le déduire nous-mêmes. On fait une première passe sur
 * la liste de quadruplets (dans l'ordre où ils ont été générés)
 * et on retient, pour chaque temporaire "Tn", quel type il a.
 */

typedef struct {
    char* name;
    DataType type;
} TempEntry;

typedef struct {
    TempEntry* entries;
    int count;
    int capacity;
} TempMap;

static void temp_map_init(TempMap* map) {
    map->capacity = 64;
    map->count = 0;
    map->entries = (TempEntry*)malloc(sizeof(TempEntry) * map->capacity);
}

static void temp_map_free(TempMap* map) {
    for (int i = 0; i < map->count; i++) {
        free(map->entries[i].name);
    }
    free(map->entries);
}

static DataType temp_map_get(TempMap* map, const char* name) {
    for (int i = 0; i < map->count; i++) {
        if (strcmp(map->entries[i].name, name) == 0) {
            return map->entries[i].type;
        }
    }
    return TYPE_UNKNOWN;
}

static void temp_map_set(TempMap* map, const char* name, DataType type) {
    for (int i = 0; i < map->count; i++) {
        if (strcmp(map->entries[i].name, name) == 0) {
            map->entries[i].type = type;
            return;
        }
    }
    if (map->count >= map->capacity) {
        map->capacity *= 2;
        map->entries = (TempEntry*)realloc(map->entries, sizeof(TempEntry) * map->capacity);
    }
    map->entries[map->count].name = stringDuplicate(name);
    map->entries[map->count].type = type;
    map->count++;
}

/* ========================================================= */
/*  UTILITAIRES DE RECONNAISSANCE D'ADRESSES                  */
/* ========================================================= */

static int is_numeric_literal(const char* s, int* has_dot) {
    if (!s || !*s) return 0;
    const char* p = s;
    if (*p == '-') p++;
    if (!isdigit((unsigned char)*p)) return 0;
    *has_dot = 0;
    for (; *p; p++) {
        if (*p == '.') *has_dot = 1;
        else if (!isdigit((unsigned char)*p)) return 0;
    }
    return 1;
}

/* Déduit le type d'une "addr" (nom de variable, temporaire ou littéral) */
static DataType resolve_type(const char* addr, SymbolTable* table, TempMap* tmap) {
    if (!addr) return TYPE_UNKNOWN;

    if (addr[0] == '\'') return TYPE_CHAR;
    if (addr[0] == '"')  return TYPE_SIGMA;
    if (strcmp(addr, "true") == 0 || strcmp(addr, "false") == 0) return TYPE_B;

    int has_dot;
    if (is_numeric_literal(addr, &has_dot)) {
        return has_dot ? TYPE_R : TYPE_Z;
    }

    /* Priorité à la table des symboles : si le symbole y est encore
       déclaré (portée pas encore fermée), c'est la source de vérité. */
    if (table) {
        SymbolEntry* e = find_symbol(table, addr);
        if (e) return e->type;
    }

    /* Sinon : un temporaire, ou une variable dont on a déjà déduit
       le type précédemment (ex: portée fermée avant le codegen). */
    return temp_map_get(tmap, addr);
}

/* Opérations dont le champ "result" désigne une variable qui reçoit
   une valeur (par opposition à une cible de branchement, comme dans
   BR/BZ/BG..., ou un résultat non pertinent comme WRITE/WRITELN). */
static int is_assigning_op(QuadOp op) {
    switch (op) {
        case QUAD_ADD: case QUAD_SUB: case QUAD_MUL: case QUAD_DIV:
        case QUAD_DIV_INT: case QUAD_MOD: case QUAD_POW: case QUAD_NEG:
        case QUAD_AND: case QUAD_OR: case QUAD_NOT: case QUAD_XOR:
        case QUAD_EQ: case QUAD_NEQ: case QUAD_LT: case QUAD_GT:
        case QUAD_LEQ: case QUAD_GEQ:
        case QUAD_ASSIGN:
        case QUAD_SIN: case QUAD_COS: case QUAD_EXP: case QUAD_LOG:
        case QUAD_SQRT: case QUAD_ABS: case QUAD_FLOOR: case QUAD_CEIL:
        case QUAD_ROUND: case QUAD_RE: case QUAD_IM: case QUAD_ARG:
        case QUAD_MAJUSCULES: case QUAD_MINUSCULES:
        case QUAD_READ:
            return 1;
        default:
            return 0;
    }
}

/* ========================================================= */
/*  PASSE 1 : INFÉRENCE DES TYPES MANQUANTS                    */
/* ========================================================= */
/*
 * Deux catégories de noms doivent être déclarées "à la main" en C
 * car elles n'apparaissent pas (ou plus) dans la table des symboles :
 *   1) Les temporaires générés par le compilateur (T0, T1, ...)
 *   2) Les variables MathLang dont le scope a déjà été fermé au
 *      moment où on génère le code (ex: la variable de boucle d'un
 *      POUR, retirée de la table par exit_scope() après le parsing).
 * On les traite de la même façon : si le nom n'est pas trouvable
 * dans la table des symboles, on déduit son type depuis l'opération
 * qui l'a produit et on le mémorise dans tmap pour le déclarer plus
 * tard.
 */
static void infer_types_pass(const QuadList* list, SymbolTable* table, TempMap* tmap) {
    for (int i = 0; i < list->count; i++) {
        const Quadruplet* q = &list->quads[i];
        if (!q->result || !is_assigning_op(q->op)) continue;

        /* Toujours déclaré dans la table : rien à déduire, on ne le
           re-déclare pas nous-mêmes (évite les doublons). */
        if (table && find_symbol(table, q->result)) continue;

        DataType t1 = resolve_type(q->arg1, table, tmap);
        DataType t2 = resolve_type(q->arg2, table, tmap);
        DataType result_type;

        switch (q->op) {
            case QUAD_ADD:
                if (t1 == TYPE_SIGMA || t2 == TYPE_SIGMA) {
                    result_type = TYPE_SIGMA; /* concaténation de chaînes */
                } else {
                    result_type = (t1 == TYPE_R || t2 == TYPE_R) ? TYPE_R : TYPE_Z;
                }
                break;

            case QUAD_SUB:
            case QUAD_MUL:
            case QUAD_MOD:
            case QUAD_POW:
                result_type = (t1 == TYPE_R || t2 == TYPE_R) ? TYPE_R : TYPE_Z;
                break;

            case QUAD_DIV:
                result_type = TYPE_R;
                break;

            case QUAD_DIV_INT:
                result_type = TYPE_Z;
                break;

            case QUAD_NEG:
            case QUAD_ABS:
                result_type = (t1 == TYPE_UNKNOWN) ? TYPE_R : t1;
                break;

            case QUAD_ASSIGN:
                result_type = (t1 == TYPE_UNKNOWN) ? TYPE_R : t1;
                break;

            case QUAD_AND:
            case QUAD_OR:
            case QUAD_NOT:
            case QUAD_XOR:
            case QUAD_EQ:
            case QUAD_NEQ:
            case QUAD_LT:
            case QUAD_GT:
            case QUAD_LEQ:
            case QUAD_GEQ:
                result_type = TYPE_B;
                break;

            case QUAD_SIN:
            case QUAD_COS:
            case QUAD_EXP:
            case QUAD_LOG:
            case QUAD_SQRT:
            case QUAD_FLOOR:
            case QUAD_CEIL:
            case QUAD_ROUND:
            case QUAD_RE:
            case QUAD_IM:
            case QUAD_ARG:
                result_type = TYPE_R;
                break;

            case QUAD_MAJUSCULES:
            case QUAD_MINUSCULES:
                result_type = TYPE_SIGMA;
                break;

            case QUAD_READ:
                /* On ne peut rien déduire d'un READ isolé : on garde
                   ce qu'on connaît déjà de cette variable si on l'a
                   déjà vue, sinon on retombe sur R par défaut. */
                result_type = temp_map_get(tmap, q->result);
                if (result_type == TYPE_UNKNOWN) result_type = TYPE_R;
                break;

            default:
                /* Choix prudent par défaut pour ne rien casser à la compilation C */
                result_type = TYPE_R;
                break;
        }

        temp_map_set(tmap, q->result, result_type);
    }
}

/* ========================================================= */
/*  TYPES C CORRESPONDANTS                                    */
/* ========================================================= */

static const char* get_c_type(DataType type) {
    switch (type) {
        case TYPE_Z:     return "long";
        case TYPE_R:     return "double";
        case TYPE_B:     return "int";
        case TYPE_CHAR:  return "char";
        case TYPE_SIGMA: return "char*";
        case TYPE_C:     return "double"; /* TODO étape 6 : struct complexe */
        default:         return "double"; /* valeur par défaut prudente */
    }
}

/* ========================================================= */
/*  ÉMISSION DES DÉCLARATIONS                                 */
/* ========================================================= */

static void emit_declarations(FILE* out, SymbolTable* table, TempMap* tmap) {
    fprintf(out, "    /* --- Variables declarees dans MathLang --- */\n");
    if (table) {
        for (int i = 0; i < HASH_TABLE_SIZE; i++) {
            SymbolEntry* e = table->entries[i];
            while (e) {
                if (e->category == SYMBOL_VARIABLE ||
                    e->category == SYMBOL_CONSTANT ||
                    e->category == SYMBOL_PARAMETER) {
                    fprintf(out, "    %s %s;\n", get_c_type(e->type), e->name);
                    if (e->type == TYPE_SIGMA) {
                        fprintf(out, "    %s = NULL;\n", e->name);
                    }
                }
                e = e->next;
            }
        }
    }

    fprintf(out, "\n    /* --- Temporaires du compilateur et variables dont la portee --- */\n");
    fprintf(out, "    /* --- avait deja ete fermee au moment du codegen           --- */\n");
    for (int i = 0; i < tmap->count; i++) {
        fprintf(out, "    %s %s;\n", get_c_type(tmap->entries[i].type), tmap->entries[i].name);
    }
    fprintf(out, "\n");
}

/* ========================================================= */
/*  ÉMISSION D'UN AFFICHAGE (WRITE) SELON LE TYPE              */
/* ========================================================= */

static void emit_write(FILE* out, const char* addr, DataType type) {
    switch (type) {
        case TYPE_Z:
            fprintf(out, "    printf(\"%%ld\", %s);\n", addr);
            break;
        case TYPE_R:
            fprintf(out, "    printf(\"%%g\", %s);\n", addr);
            break;
        case TYPE_B:
            fprintf(out, "    printf(\"%%s\", (%s) ? \"true\" : \"false\");\n", addr);
            break;
        case TYPE_CHAR:
            fprintf(out, "    printf(\"%%c\", %s);\n", addr);
            break;
        case TYPE_SIGMA:
            fprintf(out, "    printf(\"%%s\", %s);\n", addr);
            break;
        default:
            fprintf(out, "    printf(\"%%g\", (double)(%s));\n", addr);
            break;
    }
}

/* ========================================================= */
/*  TRADUCTION D'UN QUADRUPLET EN C                            */
/* ========================================================= */

static void translate_quad(FILE* out, const Quadruplet* q, SymbolTable* table, TempMap* tmap) {
    switch (q->op) {

        /* --- Arithmétique --- */
        case QUAD_ADD: {
            DataType t1 = resolve_type(q->arg1, table, tmap);
            DataType t2 = resolve_type(q->arg2, table, tmap);
            if (t1 == TYPE_SIGMA || t2 == TYPE_SIGMA) {
                /* Concaténation de chaînes : on alloue un nouveau buffer */
                fprintf(out,
                    "    %s = malloc(strlen(%s) + strlen(%s) + 1);\n"
                    "    strcpy(%s, %s);\n"
                    "    strcat(%s, %s);\n",
                    q->result, q->arg1, q->arg2,
                    q->result, q->arg1,
                    q->result, q->arg2);
            } else {
                fprintf(out, "    %s = %s + %s;\n", q->result, q->arg1, q->arg2);
            }
            break;
        }
        case QUAD_SUB:
            fprintf(out, "    %s = %s - %s;\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_MUL:
            fprintf(out, "    %s = %s * %s;\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_DIV:
            fprintf(out, "    %s = (double)(%s) / (double)(%s);\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_DIV_INT:
            fprintf(out, "    %s = (long)(%s) / (long)(%s);\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_MOD:
            fprintf(out, "    %s = (long)(%s) %% (long)(%s);\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_POW:
            fprintf(out, "    %s = pow((double)(%s), (double)(%s));\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_NEG:
            fprintf(out, "    %s = -(%s);\n", q->result, q->arg1);
            break;

        /* --- Logique --- */
        case QUAD_AND:
            fprintf(out, "    %s = (%s) && (%s);\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_OR:
            fprintf(out, "    %s = (%s) || (%s);\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_NOT:
            fprintf(out, "    %s = !(%s);\n", q->result, q->arg1);
            break;
        case QUAD_XOR:
            fprintf(out, "    %s = (!!(%s)) != (!!(%s));\n", q->result, q->arg1, q->arg2);
            break;

        /* --- Comparaisons (rarement matérialisées, cf expr_to_addr) --- */
        case QUAD_EQ:
            fprintf(out, "    %s = (%s) == (%s);\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_NEQ:
            fprintf(out, "    %s = (%s) != (%s);\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_LT:
            fprintf(out, "    %s = (%s) < (%s);\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_GT:
            fprintf(out, "    %s = (%s) > (%s);\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_LEQ:
            fprintf(out, "    %s = (%s) <= (%s);\n", q->result, q->arg1, q->arg2);
            break;
        case QUAD_GEQ:
            fprintf(out, "    %s = (%s) >= (%s);\n", q->result, q->arg1, q->arg2);
            break;

        /* --- Affectation --- */
        case QUAD_ASSIGN:
            fprintf(out, "    %s = %s;\n", q->result, q->arg1);
            break;

        /* --- Branchements inconditionnels et conditionnels --- */
        case QUAD_BR:
            fprintf(out, "    goto L%s;\n", q->result);
            break;
        case QUAD_BZ:
            fprintf(out, "    if (!(%s)) goto L%s;\n", q->arg1, q->result);
            break;
        case QUAD_BNZ:
            fprintf(out, "    if (%s) goto L%s;\n", q->arg1, q->result);
            break;
        case QUAD_BG:
            fprintf(out, "    if ((%s) > (%s)) goto L%s;\n", q->arg1, q->arg2, q->result);
            break;
        case QUAD_BGE:
            fprintf(out, "    if ((%s) >= (%s)) goto L%s;\n", q->arg1, q->arg2, q->result);
            break;
        case QUAD_BL:
            fprintf(out, "    if ((%s) < (%s)) goto L%s;\n", q->arg1, q->arg2, q->result);
            break;
        case QUAD_BLE:
            fprintf(out, "    if ((%s) <= (%s)) goto L%s;\n", q->arg1, q->arg2, q->result);
            break;
        case QUAD_BE:
            fprintf(out, "    if ((%s) == (%s)) goto L%s;\n", q->arg1, q->arg2, q->result);
            break;
        case QUAD_BNE:
            fprintf(out, "    if ((%s) != (%s)) goto L%s;\n", q->arg1, q->arg2, q->result);
            break;

        /* --- Fonctions mathématiques --- */
        case QUAD_SIN:
            fprintf(out, "    %s = sin((double)(%s));\n", q->result, q->arg1);
            break;
        case QUAD_COS:
            fprintf(out, "    %s = cos((double)(%s));\n", q->result, q->arg1);
            break;
        case QUAD_EXP:
            fprintf(out, "    %s = exp((double)(%s));\n", q->result, q->arg1);
            break;
        case QUAD_LOG:
            fprintf(out, "    %s = log((double)(%s));\n", q->result, q->arg1);
            break;
        case QUAD_SQRT:
            fprintf(out, "    %s = sqrt((double)(%s));\n", q->result, q->arg1);
            break;
        case QUAD_ABS:
            fprintf(out, "    %s = fabs((double)(%s));\n", q->result, q->arg1);
            break;
        case QUAD_FLOOR:
            fprintf(out, "    %s = floor((double)(%s));\n", q->result, q->arg1);
            break;
        case QUAD_CEIL:
            fprintf(out, "    %s = ceil((double)(%s));\n", q->result, q->arg1);
            break;
        case QUAD_ROUND:
            fprintf(out, "    %s = round((double)(%s));\n", q->result, q->arg1);
            break;

        case QUAD_RE:
        case QUAD_IM:
        case QUAD_ARG:
            fprintf(out, "    /* TODO etape 6 : nombres complexes non geres */\n");
            fprintf(out, "    %s = 0.0;\n", q->result);
            break;

        /* --- Fonctions chaînes --- */
        case QUAD_MAJUSCULES:
            fprintf(out,
                "    %s = strdup(%s);\n"
                "    for (char* p = %s; *p; p++) *p = (char)toupper((unsigned char)*p);\n",
                q->result, q->arg1, q->result);
            break;
        case QUAD_MINUSCULES:
            fprintf(out,
                "    %s = strdup(%s);\n"
                "    for (char* p = %s; *p; p++) *p = (char)tolower((unsigned char)*p);\n",
                q->result, q->arg1, q->result);
            break;

        /* --- Entrées / sorties --- */
        case QUAD_READ: {
            DataType t = resolve_type(q->result, table, tmap);
            if (t == TYPE_Z)        fprintf(out, "    scanf(\"%%ld\", &%s);\n", q->result);
            else if (t == TYPE_R)   fprintf(out, "    scanf(\"%%lf\", &%s);\n", q->result);
            else if (t == TYPE_CHAR) fprintf(out, "    scanf(\" %%c\", &%s);\n", q->result);
            else fprintf(out, "    /* TODO : lecture non geree pour ce type */\n");
            break;
        }
        case QUAD_WRITE: {
            DataType t = resolve_type(q->arg1, table, tmap);
            emit_write(out, q->arg1, t);
            break;
        }
        case QUAD_WRITELN:
            fprintf(out, "    printf(\"\\n\");\n");
            break;

        /* --- Fonctions (traitées plus tard, etape 4) --- */
        case QUAD_RETURN:
            fprintf(out, "    return %s;\n", q->arg1 ? q->arg1 : "0");
            break;
        case QUAD_PARAM:
        case QUAD_CALL:
            fprintf(out, "    /* TODO etape 4 : appels de fonctions non geres */\n");
            break;

        case QUAD_LABEL:
        case QUAD_NOP:
            fprintf(out, "    ;\n");
            break;

        default:
            fprintf(out, "    /* TODO : quadruplet non traduit (%s) */\n", quadOpToString(q->op));
            break;
    }
}

/* ========================================================= */
/*  POINT D'ENTRÉE PRINCIPAL                                  */
/* ========================================================= */

void generate_c_code(FILE* out, QuadList* list, SymbolTable* table) {
    if (!out || !list) return;

    TempMap tmap;
    temp_map_init(&tmap);
    infer_types_pass(list, table, &tmap);

    fprintf(out, "/* ===================================================== */\n");
    fprintf(out, "/* Fichier genere automatiquement par le compilateur      */\n");
    fprintf(out, "/* MathLang -> C. Ne pas modifier a la main.              */\n");
    fprintf(out, "/* ===================================================== */\n\n");
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "#include <string.h>\n");
    fprintf(out, "#include <math.h>\n");
    fprintf(out, "#include <ctype.h>\n");
    fprintf(out, "#include <stdbool.h>\n\n");
    fprintf(out, "int main(void) {\n");

    emit_declarations(out, table, &tmap);

    for (int i = 0; i < list->count; i++) {
        fprintf(out, "L%d:;\n", i); /* le ';' evite l'erreur "label sans instruction" en C */
        translate_quad(out, &list->quads[i], table, &tmap);
    }

    /* Un branchement (fin de IF/TANT QUE/POUR...) peut pointer vers
       nextQuad() au moment du backpatch, c'est-a-dire un indice qui
       vaut exactement list->count : une etiquette juste APRES le
       dernier quadruplet. Sans elle, gcc refuse le "goto" correspondant. */
    fprintf(out, "L%d:;\n", list->count);

    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");

    temp_map_free(&tmap);
}
#include "indice.h"
#include "parser.h"
#include <stdlib.h>

static TablaIndice indice_global = {0};

static void indice_reconstruir_desde_db(TablaIndice *indice);

unsigned int indice_hash(const char *tabla, const char *clave) {
    unsigned int hash = 5381;
    const char *str = tabla;
    int phase = 0;
    
    while (1) {
        int c = *str++;
        if (c == '\0') {
            if (phase == 0) {
                str = clave;
                phase = 1;
                continue;
            } else {
                break;
            }
        }
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash % INDICE_HASH_SIZE;
}

void indice_inicializar(TablaIndice *indice, const char *nombre_bd) {
    for (int i = 0; i < INDICE_HASH_SIZE; i++) {
        indice->tablas[i] = NULL;
    }
    indice->total_entradas = 0;
    strncpy(indice->nombre_bd, nombre_bd, MAX_CAMPO - 1);
}

void indice_construir(TablaIndice *indice) {
    char ruta[512];
    snprintf(ruta, sizeof(ruta), "data/%s.idx", indice->nombre_bd);
    
    FILE *fp = fopen(ruta, "r");
    if (!fp) {
        indice_reconstruir_desde_db(indice);
        return;
    }
    
    indice_liberar(indice);
    
    char linea[MAX_LINEA];
    while (fgets(linea, MAX_LINEA, fp)) {
        linea[strcspn(linea, "\n")] = 0;
        if (linea[0] == '#' || strlen(linea) == 0) continue;
        
        char tabla[MAX_CAMPO], clave[MAX_CAMPO];
        long offset;
        if (sscanf(linea, "%[^:]:%[^:]:%ld", tabla, clave, &offset) == 3) {
            EntradaIndice *entrada = malloc(sizeof(EntradaIndice));
            if (entrada) {
                strncpy(entrada->tabla, tabla, MAX_CAMPO - 1);
                strncpy(entrada->clave, clave, MAX_CAMPO - 1);
                entrada->offset = offset;
                
                unsigned int hash = indice_hash(tabla, clave);
                entrada->siguiente = indice->tablas[hash];
                indice->tablas[hash] = entrada;
                indice->total_entradas++;
            }
        }
    }
    
    fclose(fp);
}

static void indice_reconstruir_desde_db(TablaIndice *indice) {
    char db_ruta[512];
    snprintf(db_ruta, sizeof(db_ruta), "data/%s.db", indice->nombre_bd);
    
    FILE *fp = fopen(db_ruta, "r");
    if (!fp) return;
    
    indice_liberar(indice);
    
    char linea[MAX_LINEA];
    long offset = 0;
    while (fgets(linea, MAX_LINEA, fp)) {
        linea[strcspn(linea, "\n")] = 0;
        if (linea[0] == '#' || strlen(linea) == 0) {
            offset += strlen(linea) + 1;
            continue;
        }
        
        char tabla[MAX_CAMPO], clave[MAX_CAMPO];
        if (sscanf(linea, "%[^:]:%[^:]", tabla, clave) >= 2) {
            EntradaIndice *entrada = malloc(sizeof(EntradaIndice));
            if (entrada) {
                strncpy(entrada->tabla, tabla, MAX_CAMPO - 1);
                strncpy(entrada->clave, clave, MAX_CAMPO - 1);
                entrada->offset = offset;
                
                unsigned int hash = indice_hash(tabla, clave);
                entrada->siguiente = indice->tablas[hash];
                indice->tablas[hash] = entrada;
                indice->total_entradas++;
            }
        }
        
        offset += strlen(linea) + 1;
    }
    
    fclose(fp);
    indice_guardar(indice);
}

void indice_guardar(TablaIndice *indice) {
    char ruta[512];
    snprintf(ruta, sizeof(ruta), "data/%s.idx", indice->nombre_bd);
    
    FILE *fp = fopen(ruta, "w");
    if (!fp) return;
    
    fprintf(fp, "# MiniDB Index - BD: %s\n", indice->nombre_bd);
    
    for (int i = 0; i < INDICE_HASH_SIZE; i++) {
        EntradaIndice *entrada = indice->tablas[i];
        while (entrada) {
            fprintf(fp, "%s:%s:%ld\n", entrada->tabla, entrada->clave, entrada->offset);
            entrada = entrada->siguiente;
        }
    }
    
    fclose(fp);
}

long indice_buscar(TablaIndice *indice, const char *tabla, const char *clave) {
    unsigned int hash = indice_hash(tabla, clave);
    EntradaIndice *entrada = indice->tablas[hash];
    
    while (entrada) {
        if (strcmp(entrada->tabla, tabla) == 0 &&
            strcmp(entrada->clave, clave) == 0) {
            return entrada->offset;
        }
        entrada = entrada->siguiente;
    }
    
    return -1;
}

int indice_insertar(TablaIndice *indice, const char *tabla, const char *clave, long offset) {
    unsigned int hash = indice_hash(tabla, clave);
    EntradaIndice *entrada = malloc(sizeof(EntradaIndice));
    if (!entrada) return -1;
    
    strncpy(entrada->tabla, tabla, MAX_CAMPO - 1);
    strncpy(entrada->clave, clave, MAX_CAMPO - 1);
    entrada->offset = offset;
    entrada->siguiente = indice->tablas[hash];
    indice->tablas[hash] = entrada;
    indice->total_entradas++;
    
    return 0;
}

int indice_actualizar(TablaIndice *indice, const char *tabla, const char *clave, long offset) {
    unsigned int hash = indice_hash(tabla, clave);
    EntradaIndice *entrada = indice->tablas[hash];
    
    while (entrada) {
        if (strcmp(entrada->tabla, tabla) == 0 &&
            strcmp(entrada->clave, clave) == 0) {
            entrada->offset = offset;
            return 0;
        }
        entrada = entrada->siguiente;
    }
    
    return indice_insertar(indice, tabla, clave, offset);
}

int indice_eliminar(TablaIndice *indice, const char *tabla, const char *clave) {
    unsigned int hash = indice_hash(tabla, clave);
    EntradaIndice *entrada = indice->tablas[hash];
    EntradaIndice *anterior = NULL;
    
    while (entrada) {
        if (strcmp(entrada->tabla, tabla) == 0 &&
            strcmp(entrada->clave, clave) == 0) {
            if (anterior) {
                anterior->siguiente = entrada->siguiente;
            } else {
                indice->tablas[hash] = entrada->siguiente;
            }
            free(entrada);
            indice->total_entradas--;
            return 0;
        }
        anterior = entrada;
        entrada = entrada->siguiente;
    }
    
    return -1;
}

void indice_liberar(TablaIndice *indice) {
    for (int i = 0; i < INDICE_HASH_SIZE; i++) {
        EntradaIndice *entrada = indice->tablas[i];
        while (entrada) {
            EntradaIndice *sig = entrada->siguiente;
            free(entrada);
            entrada = sig;
        }
        indice->tablas[i] = NULL;
    }
    indice->total_entradas = 0;
}

TablaIndice* indice_obtener_global() {
    return &indice_global;
}

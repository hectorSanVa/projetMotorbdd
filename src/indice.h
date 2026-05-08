#ifndef INDICE_H
#define INDICE_H

#include "definiciones.h"

#define INDICE_HASH_SIZE 256

typedef struct EntradaIndice {
    char tabla[MAX_CAMPO];
    char clave[MAX_CAMPO];
    long offset;
    struct EntradaIndice *siguiente;
} EntradaIndice;

typedef struct {
    EntradaIndice *tablas[INDICE_HASH_SIZE];
    int total_entradas;
    char nombre_bd[MAX_CAMPO];
} TablaIndice;

unsigned int indice_hash(const char *tabla, const char *clave);
void indice_inicializar(TablaIndice *indice, const char *nombre_bd);
void indice_construir(TablaIndice *indice);
void indice_guardar(TablaIndice *indice);
long indice_buscar(TablaIndice *indice, const char *tabla, const char *clave);
int indice_insertar(TablaIndice *indice, const char *tabla, const char *clave, long offset);
int indice_actualizar(TablaIndice *indice, const char *tabla, const char *clave, long offset);
int indice_eliminar(TablaIndice *indice, const char *tabla, const char *clave);
void indice_liberar(TablaIndice *indice);

TablaIndice* indice_obtener_global();

#endif

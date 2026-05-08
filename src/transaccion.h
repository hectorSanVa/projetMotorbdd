#ifndef TRANSACCION_H
#define TRANSACCION_H

#include "definiciones.h"

#define MAX_BUFFER 1000

typedef struct {
    char operacion;
    char tabla[MAX_CAMPO];
    char clave[MAX_CAMPO];
    int valor;
} Cambio;

typedef struct {
    int activa;
    int numero;
    Cambio cambios[MAX_BUFFER];
    int num_cambios;
    char backup_contenido[65536];
    int backup_size;
} BufferTransaccion;

void txn_iniciar();
void txn_agregar_cambio(char op, const char *tabla, const char *clave, int valor);
void txn_agregar_cambio_sesion(BufferTransaccion *txn, char op, const char *tabla, const char *clave, int valor);
int txn_guardar();
int txn_cancelar();
int txn_activa();

#endif

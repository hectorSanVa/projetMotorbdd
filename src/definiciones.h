#ifndef DEFINICIONES_H
#define DEFINICIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINEA 256
#define MAX_CAMPO 64
#define MAX_TABLAS 10
#define MAX_VALOR 128
#define ARCHIVO_DB "data/tienda.db"
#define ARCHIVO_LOG "data/transaccion.log"

#define C_VERDE   "\033[32m"
#define C_ROJO    "\033[31m"
#define C_AMARILLO "\033[33m"
#define C_AZUL    "\033[34m"
#define C_MAGENTA "\033[35m"
#define C_CYAN    "\033[36m"
#define C_BLANCO  "\033[37m"
#define C_NEGRITA "\033[1m"
#define C_RESET   "\033[0m"

#define OK    C_VERDE "[OK]" C_RESET
#define ERROR C_ROJO "[ERROR]" C_RESET
#define WARN  C_AMARILLO "[WARN]" C_RESET
#define INFO  C_AZUL "[INFO]" C_RESET

typedef struct {
    char nombre[MAX_CAMPO];
    char clave[MAX_CAMPO];
    int tipo;
    union {
        int entero;
        char texto[MAX_VALOR];
    } valor;
} Registro;

typedef enum {
    TIPO_ENTERO = 0,
    TIPO_TEXTO = 1
} TipoDato;

typedef enum {
    ESTADO_ESPERA,
    ESTADO_TRANSACCION_ACTIVA,
    ESTADO_TRANSACCION_FALLIDA
} EstadoTransaccion;

typedef struct {
    EstadoTransaccion estado;
    int numero_transaccion;
    bool hay_cambios;
} GestorTransaccion;

#endif
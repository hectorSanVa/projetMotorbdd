#ifndef PARSER_H
#define PARSER_H

#include "definiciones.h"

extern GestorTransaccion txn_actual;
extern char global_salida[8192];
extern char *buffer_salida;
extern int tamano_buffer;

void procesar_comando(char *entrada);
const char* obtener_salida();
void limpiar_salida();
void salida_set_buffer(char *buf, int tam);
char* salida_get_buffer();
void out(const char *format, ...);
int parsear_agregar(char *entrada);
int parsear_ver(char *entrada);
int parsear_modificar(char *entrada);
int parsear_borrar(char *entrada);
int parsear_backup();
int parsear_crearbd(char *entrada);
int parsear_usar(char *entrada);
int parsear_mostrarbd();
int parsear_eliminarbd(char *entrada);
int parsear_infobd();
int parsear_listar_tablas();
int parsear_limpiar();
const char* obtener_bd_actual();
void cambiar_bd(const char *nueva_bd);

#endif

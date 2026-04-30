#ifndef GESTOR_ARCH_H
#define GESTOR_ARCH_H

#include "definiciones.h"

int iniciar_archivo();
int escribir_registro(const char *tabla, const char *clave, const char *valor);
int escribir_registro_entero(const char *tabla, const char *clave, int valor);
int escribir_registro_texto(const char *tabla, const char *clave, const char *valor);
int leer_todos(const char *tabla);
int buscar_registro(const char *tabla, const char *clave, Registro *reg);
int modificar_registro(const char *tabla, const char *clave, const char *nuevo_valor);
int modificar_registro_entero(const char *tabla, const char *clave, int nuevo_valor);
int modificar_registro_texto(const char *tabla, const char *clave, const char *nuevo_valor);
int borrar_registro(const char *tabla, const char *clave);
int existe_tabla(const char *tabla);
int crear_tabla_en_bd(const char *tabla);
const char* obtener_ruta_bd();
const char* obtener_bd_actual();
void cambiar_bd(const char *nueva_bd);

#endif
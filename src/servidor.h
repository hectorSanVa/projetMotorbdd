#ifndef SERVIDOR_H
#define SERVIDOR_H

#include "definiciones.h"

#define PUERTO_HTTP 8080
#define BUFFER_HTTP 4096

int iniciar_servidor_http(int puerto);
void *manejar_cliente(void *arg);
char *procesar_peticion_http(const char *peticion);

#endif
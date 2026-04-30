#ifndef PROCESOS_H
#define PROCESOS_H

#include <signal.h>

void configurar_senales();
void iniciar_backup_automatico();
void realizar_backup();
void procesar_senales(int sig);

#endif
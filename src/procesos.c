#include "procesos.h"
#include "definiciones.h"
#include "parser.h"
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

static volatile int sigint_recibido = 0;

void procesar_senales(int sig) {
    if (sig == SIGINT) {
        sigint_recibido = 1;
    }
}

void configurar_senales() {
    struct sigaction sa;
    sa.sa_handler = procesar_senales;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

void realizar_backup() {
    time_t ahora = time(NULL);
    char nombre_backup[256];
    struct tm *tm_info = localtime(&ahora);
    char fecha[64];
    strftime(fecha, sizeof(fecha), "%Y%m%d_%H%M%S", tm_info);
    
    snprintf(nombre_backup, sizeof(nombre_backup), "data/backup_%s.db", fecha);
    
    FILE *origen = fopen(ARCHIVO_DB, "r");
    if (!origen) {
        out("[BACKUP] No hay datos para respaldar.\n");
        return;
    }
    
    FILE *destino = fopen(nombre_backup, "w");
    if (!destino) {
        fclose(origen);
        out("[BACKUP] Error al crear backup.\n");
        return;
    }
    
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), origen)) > 0) {
        fwrite(buffer, 1, bytes, destino);
    }
    
    fclose(origen);
    fclose(destino);
    
    out("[BACKUP] Respaldado: %s\n", nombre_backup);
}

void iniciar_backup_automatico() {
    pid_t pid = fork();
    
    if (pid < 0) {
        out("[PROCESO] Error al crear proceso de backup.\n");
        return;
    }
    
    if (pid == 0) {
        out("[PROCESO] Proceso hijo de backup iniciado (PID: %d)\n", getpid());
        
        while (!sigint_recibido) {
            sleep(30);
            
            FILE *fp = fopen(ARCHIVO_DB, "r");
            if (fp) {
                fseek(fp, 0, SEEK_END);
                if (ftell(fp) > 0) {
                    fclose(fp);
                    time_t ahora = time(NULL);
                    char nombre_backup[256];
                    struct tm *tm_info = localtime(&ahora);
                    char fecha[64];
                    strftime(fecha, sizeof(fecha), "%Y%m%d_%H%M%S", tm_info);
                    snprintf(nombre_backup, sizeof(nombre_backup), "data/backup_%s.db", fecha);
                    
                    char cmd[512];
                    snprintf(cmd, sizeof(cmd), "cp %s %s 2>/dev/null", ARCHIVO_DB, nombre_backup);
                    system(cmd);
                } else {
                    fclose(fp);
                }
            }
        }
        
        exit(0);
    } else {
        out("[PROCESO] Proceso padre (PID: %d), hijo backup: %d\n", getpid(), pid);
    }
}

void esperar_proceso_backup() {
    wait(NULL);
}
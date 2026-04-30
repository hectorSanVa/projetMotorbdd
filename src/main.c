#include "definiciones.h"
#include "parser.h"
#include "gestor_arch.h"
#include "servidor.h"
#include "procesos.h"

void mostrar_banner();
void iniciar_sistema();

static int modo_servidor = 0;

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--web") == 0) {
        modo_servidor = 1;
    }
    
    if (!modo_servidor) {
        iniciar_sistema();
    } else {
        printf("=== MiniDB Transaccional v1.0 ===\n");
        printf("=== Modo Servidor Web ===\n\n");
        iniciar_archivo();
    }
    
    if (modo_servidor) {
        iniciar_servidor_http(PUERTO_HTTP);
    } else {
        char entrada[MAX_LINEA];
        
        while (1) {
            printf("MiniDB> ");
            fflush(stdout);
            
            if (fgets(entrada, MAX_LINEA, stdin) == NULL) {
                break;
            }
            
            entrada[strcspn(entrada, "\n")] = 0;
            
            if (strlen(entrada) == 0) {
                continue;
            }
            
            procesar_comando(entrada);
            printf("%s", obtener_salida());
            fflush(stdout);
        }
        
        printf("\nHasta luego.\n");
    }
    
    return 0;
}

void mostrar_banner() {
    printf("╔══════════════════════════════════╗\n");
    printf("║   MiniDB Transaccional v1.0     ║\n");
    printf("║   Sistema de Bases de Datos     ║\n");
    printf("╚══════════════════════════════════╝\n\n");
    printf("BD: crearbd, eliminarbd, mostrarbd, usar, infobd, listar\n");
    printf("CRUD: agregar, ver, modificar, borrar\n");
    printf("TXN: iniciar, guardar, cancelar\n");
    printf("Otros: backup, ayuda, salir\n\n");
}

void iniciar_sistema() {
    mostrar_banner();
    iniciar_archivo();
}
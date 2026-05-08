#include "definiciones.h"
#include "parser.h"
#include "gestor_arch.h"
#include "procesos.h"
#include "indice.h"

void mostrar_banner();
void iniciar_sistema();

int main(int argc, char *argv[]) {
    iniciar_sistema();
    
    char entrada[MAX_LINEA];
    
    while (1) {
        printf("MiniDB[%s]> ", obtener_bd_actual());
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
    TablaIndice *indice = indice_obtener_global();
    indice_inicializar(indice, obtener_bd_actual());
    indice_construir(indice);
}
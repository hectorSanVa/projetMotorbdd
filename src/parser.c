#include "parser.h"
#include "gestor_arch.h"
#include "transaccion.h"
#include "procesos.h"
#include <stdarg.h>
#include <ctype.h>
#include <dirent.h>

int parsear_creartabla(char *entrada);

GestorTransaccion txn_actual = {ESTADO_ESPERA, 0, false};
char global_salida[8192];
char *buffer_salida = NULL;
int tamano_buffer = 0;
static char bd_actual[MAX_CAMPO] = "tienda";

const char* obtener_bd_actual() {
    return bd_actual;
}

void cambiar_bd(const char *nueva_bd) {
    strncpy(bd_actual, nueva_bd, MAX_CAMPO - 1);
}

void salida_set_buffer(char *buf, int tam) {
    buffer_salida = buf;
    tamano_buffer = tam;
}

char* salida_get_buffer() {
    return buffer_salida ? buffer_salida : global_salida;
}

const char* obtener_salida() {
    return buffer_salida ? buffer_salida : global_salida;
}

void limpiar_salida() {
    if (buffer_salida && tamano_buffer > 0) {
        buffer_salida[0] = '\0';
    } else {
        global_salida[0] = '\0';
    }
}

void out(const char *format, ...) {
    char temp[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(temp, sizeof(temp), format, args);
    va_end(args);
    
    if (buffer_salida && tamano_buffer > 0) {
        if (strlen(buffer_salida) + strlen(temp) < (size_t)tamano_buffer) {
            strcat(buffer_salida, temp);
        }
    } else {
        strcat(global_salida, temp);
    }
}

void procesar_comando(char *entrada) {
    limpiar_salida();
    char comando[MAX_CAMPO];
    char comando_lower[MAX_CAMPO];
    
    char *delimiter = strchr(entrada, ';');
    if (delimiter) {
        *delimiter = '\0';
        if (strlen(entrada) > 0) {
            procesar_comando(entrada);
        }
        if (strlen(delimiter + 1) > 0) {
            procesar_comando(delimiter + 1);
        }
        return;
    }
    
    sscanf(entrada, "%s", comando);
    strcpy(comando_lower, comando);
    for (int i = 0; comando_lower[i]; i++) {
        comando_lower[i] = tolower(comando_lower[i]);
    }
    
    if (strcmp(comando_lower, "salir") == 0) {
        out("Saliendo...\n");
        exit(0);
    }
    
    else if (strcmp(comando_lower, "creartabla") == 0 || strcmp(comando_lower, "newtable") == 0) {
        parsear_creartabla(entrada);
    }
    
    else if (strcmp(comando_lower, "ayuda") == 0) {
        out("\n=== Comandos Disponibles ===\n");
        out("\n--- Gestion de Base de Datos ---\n");
        out("  CREARBD <nombre>    - Crear nueva base de datos\n");
        out("  ELIMINARBD <nombre> - Eliminar una base de datos\n");
        out("  MOSTRARBD          - Ver todas las bases de datos\n");
        out("  USAR <nombre>      - Cambiar a otra base de datos\n");
        out("  INFOBD             - Ver informacion de la BD actual\n");
        out("\n--- Tablas ---\n");
        out("  LISTAR             - Ver todas las tablas de la BD actual\n");
        out("\n--- CRUD ---\n");
        out("  CREARTABLA <nombre>    - Crear tabla\n");
        out("  AGREGAR <t> <c> <v> - Agregar registro\n");
        out("  VER <tabla>         - Ver registros\n");
        out("  MODIFICAR <t> <c> <v> - Modificar registro\n");
        out("  BORRAR <tabla> <clave> - Eliminar registro\n");
        out("\n--- Transacciones ---\n");
        out("  INICIAR                   - Iniciar transaccion (BEGIN)\n");
        out("  GUARDAR                   - Guardar cambios (COMMIT)\n");
        out("  CANCELAR                  - Cancelar cambios (ROLLBACK)\n");
        out("\n--- Otros ---\n");
        out("  BACKUP                    - Crear backup manual\n");
        out("  LIMPIAR                   - Borrar todos los datos\n");
        out("  SALIR                     - Salir del programa\n");
        out("\n--- Delimitador ---\n");
        out("  Usa ; para ejecutar varios comandos en una linea\n");
        out("  Ejemplo: agregar u ana 22; ver u\n");
    }
    
    else if (strcmp(comando_lower, "agregar") == 0) {
        parsear_agregar(entrada);
    }
    
    else if (strcmp(comando_lower, "ver") == 0) {
        parsear_ver(entrada);
    }
    
    else if (strcmp(comando_lower, "modificar") == 0) {
        parsear_modificar(entrada);
    }
    
    else if (strcmp(comando_lower, "borrar") == 0) {
        parsear_borrar(entrada);
    }
    
    else if (strcmp(comando_lower, "iniciar") == 0) {
        txn_iniciar();
    }
    
    else if (strcmp(comando_lower, "guardar") == 0) {
        txn_guardar();
    }
    
    else if (strcmp(comando_lower, "cancelar") == 0) {
        txn_cancelar();
    }
    
    else if (strcmp(comando_lower, "insert") == 0 || strcmp(comando_lower, "add") == 0) {
        parsear_agregar(entrada);
    }
    
    else if (strcmp(comando_lower, "select") == 0 || strcmp(comando_lower, "list") == 0 || strcmp(comando_lower, "read") == 0) {
        parsear_ver(entrada);
    }
    
    else if (strcmp(comando_lower, "update") == 0 || strcmp(comando_lower, "modify") == 0) {
        parsear_modificar(entrada);
    }
    
    else if (strcmp(comando_lower, "delete") == 0 || strcmp(comando_lower, "remove") == 0) {
        parsear_borrar(entrada);
    }
    
    else if (strcmp(comando_lower, "begin") == 0 || strcmp(comando_lower, "start") == 0 || strcmp(comando_lower, "txn") == 0) {
        txn_iniciar();
    }
    
    else if (strcmp(comando_lower, "commit") == 0 || strcmp(comando_lower, "save") == 0) {
        txn_guardar();
    }
    
    else if (strcmp(comando_lower, "rollback") == 0 || strcmp(comando_lower, "abort") == 0) {
        txn_cancelar();
    }
    
    else if (strcmp(comando_lower, "backup") == 0) {
        parsear_backup();
    }
    
    else if (strcmp(comando_lower, "senales") == 0) {
        configurar_senales();
        out("[SISTEMA] Senales configuradas (Ctrl+C para interrupcion)\n");
    }
    
else if (strcmp(comando_lower, "crearbd") == 0 || strcmp(comando_lower, "creardb") == 0 || strcmp(comando_lower, "newdb") == 0) {
        parsear_crearbd(entrada);
    }
    
    else if (strcmp(comando_lower, "usar") == 0 || strcmp(comando_lower, "usedb") == 0) {
        parsear_usar(entrada);
    }
    
    else if (strcmp(comando_lower, "mostrarbd") == 0 || strcmp(comando_lower, "showdb") == 0 || strcmp(comando_lower, "listadb") == 0) {
        parsear_mostrarbd();
    }
    
    else if (strcmp(comando_lower, "eliminarbd") == 0 || strcmp(comando_lower, "dropdb") == 0 || strcmp(comando_lower, "deletedb") == 0) {
        parsear_eliminarbd(entrada);
    }
    
    else if (strcmp(comando_lower, "infobd") == 0 || strcmp(comando_lower, "status") == 0 || strcmp(comando_lower, "info") == 0) {
        parsear_infobd();
    }
    
    else if (strcmp(comando_lower, "listar") == 0 || strcmp(comando_lower, "tables") == 0 || strcmp(comando_lower, "list") == 0) {
        parsear_listar_tablas();
    }
    
    else if (strcmp(comando_lower, "limpiar") == 0 || strcmp(comando_lower, "clear") == 0 || strcmp(comando_lower, "reset") == 0) {
        out("[OK] Historial limpiado\n");
    }
    
    else if (strcmp(comando_lower, "backup") == 0 || strcmp(comando_lower, "dump") == 0 || strcmp(comando_lower, "export") == 0) {
        parsear_backup();
    }
    
else if (strcmp(comando_lower, "ayuda") == 0 || strcmp(comando_lower, "help") == 0 || strcmp(comando_lower, "?") == 0) {
        out("=== Comandos Disponibles ===\n");
        out("\n--- Gestion de Base de Datos ---\n");
        out("  CREARBD <nombre>    - Crear nueva base de datos\n");
        out("  ELIMINARBD <nombre> - Eliminar una base de datos\n");
        out("  MOSTRARBD          - Ver todas las bases de datos\n");
        out("  USAR <nombre>      - Cambiar a otra base de datos\n");
        out("  INFOBD             - Ver informacion de la BD actual\n");
        out("\n--- Tablas ---\n");
        out("  CREARTABLA <nombre> - Crear tabla\n");
        out("  LISTAR             - Ver todas las tablas de la BD actual\n");
        out("\n--- CRUD ---\n");
        out("  AGREGAR <t> <c> <v> - Agregar registro\n");
        out("  VER <tabla>         - Ver registros\n");
        out("  MODIFICAR <t> <c> <v> - Modificar registro\n");
        out("  BORRAR <tabla> <clave> - Eliminar registro\n");
        out("\n--- Transacciones ---\n");
        out("  INICIAR             - Iniciar transaccion (BEGIN)\n");
        out("  GUARDAR            - Guardar cambios (COMMIT)\n");
        out("  CANCELAR           - Cancelar cambios (ROLLBACK)\n");
        out("\n--- Utilidades ---\n");
        out("  BACKUP             - Crear backup manual\n");
        out("  LIMPIAR            - Borrar todos los datos\n");
        out("  SALIR              - Salir del programa\n");
        out("\n--- Delimitador ---\n");
        out("  Usa ; para ejecutar varios comandos en una linea\n");
        out("  Ejemplo: agregar u ana 22; ver u\n");
    }
    
    else {
        out("[ERROR] Comando desconocido: %s\n", comando);
    }
}

int parsear_agregar(char *entrada) {
    char tabla[MAX_CAMPO], clave[MAX_CAMPO];
    char valor[MAX_VALOR] = {0};
    
    char *args = entrada + 7;
    while (*args == ' ') args++;
    
    int scanned = sscanf(args, "%s %s", tabla, clave);
    
    if (scanned < 2) {
        out("[ERROR] Uso: AGREGAR <tabla> <clave> <valor>\n");
        return -1;
    }
    
    args = strchr(args, ' ');
    if (args) {
        args++;
        while (*args == ' ') args++;
        
        if (args[0] == '"') {
            char *end = strchr(args + 1, '"');
            if (end) {
                *end = '\0';
                strcpy(valor, args + 1);
            } else {
                strcpy(valor, args);
            }
        } else {
            strcpy(valor, args);
        }
    }
    
    if (strlen(valor) == 0) {
        out("[ERROR] Falta valor. Uso: AGREGAR <tabla> <clave> <valor>\n");
        return -1;
    }
    
    Registro reg;
    if (buscar_registro(tabla, clave, &reg) == 1) {
        out("[ERROR] Ya existe el registro: %s:%s\n", tabla, clave);
        return -1;
    }
    
    if (escribir_registro(tabla, clave, valor) == 0) {
        out("[OK] Registro agregado: %s:%s = %s\n", tabla, clave, valor);
        txn_agregar_cambio('A', tabla, clave, 0);
        return 0;
    }
    
    return -1;
}

int parsear_ver(char *entrada) {
    char tabla[MAX_CAMPO] = {0};
    int n = 0;
    
    char *args = entrada + 3;
    while (*args == ' ') args++;
    
    if (strlen(args) > 0) {
        n = sscanf(args, "%s", tabla);
    }
    
    if (n != 1 || strlen(tabla) == 0) {
        out("[ERROR] Falta argumento. Uso: ver <tabla>\n");
        out("Ejemplo: ver usuarios\n");
        return -1;
    }
    
    return leer_todos(tabla);
}

int parsear_modificar(char *entrada) {
    char tabla[MAX_CAMPO], clave[MAX_CAMPO];
    char valor[MAX_VALOR] = {0};
    
    char *args = entrada + 9;
    while (*args == ' ') args++;
    
    int scanned = sscanf(args, "%s %s", tabla, clave);
    
    if (scanned < 2) {
        out("[ERROR] Uso: MODIFICAR <tabla> <clave> <valor>\n");
        return -1;
    }
    
    args = strchr(args, ' ');
    if (args) {
        args++;
        while (*args == ' ') args++;
        
        if (args[0] == '"') {
            char *end = strchr(args + 1, '"');
            if (end) {
                *end = '\0';
                strcpy(valor, args + 1);
            } else {
                strcpy(valor, args);
            }
        } else {
            strcpy(valor, args);
        }
    }
    
    if (strlen(valor) == 0) {
        out("[ERROR] Falta valor. Uso: MODIFICAR <tabla> <clave> <valor>\n");
        return -1;
    }
    
    Registro reg;
    if (buscar_registro(tabla, clave, &reg) != 1) {
        out("[ERROR] No existe el registro: %s:%s\n", tabla, clave);
        return -1;
    }
    
    if (modificar_registro(tabla, clave, valor) == 1) {
        out("[OK] Registro modificado: %s:%s = %s\n", tabla, clave, valor);
        txn_agregar_cambio('M', tabla, clave, 0);
        return 0;
    }
    
    return -1;
}

int parsear_borrar(char *entrada) {
    char tabla[MAX_CAMPO], clave[MAX_CAMPO];
    
    int n = sscanf(entrada + 6, "%s %s", tabla, clave);
    
    if (n != 2) {
        printf("[ERROR] Uso: BORRAR <tabla> <clave>\n");
        return -1;
    }
    
    Registro reg;
    if (buscar_registro(tabla, clave, &reg) != 1) {
        printf("[ERROR] No existe el registro: %s:%s\n", tabla, clave);
        return -1;
    }
    
    if (borrar_registro(tabla, clave) == 1) {
        out("[OK] Registro borrado: %s:%s\n", tabla, clave);
        txn_agregar_cambio('B', tabla, clave, 0);
        return 0;
    }
    
    return -1;
}

int parsear_backup() {
    realizar_backup();
    return 0;
}

int parsear_creartabla(char *entrada) {
    char tabla[MAX_CAMPO];
    int n = sscanf(entrada + 10, "%s", tabla);
    
    if (n != 1) {
        out("[ERROR] Uso: creartabla <nombre>\n");
        return -1;
    }
    
    int resultado = crear_tabla_en_bd(tabla);
    if (resultado == 1) {
        out("[OK] Tabla '%s' creada\n", tabla);
    }
    
    return resultado;
}

int parsear_crearbd(char *entrada) {
    char nombre_bd[MAX_CAMPO];
    int n = sscanf(entrada + 7, "%s", nombre_bd);
    
    if (n != 1) {
        out("[ERROR] Uso: crearbd <nombre>\n");
        out("Ejemplo: crearbd mi_empresa\n");
        return -1;
    }
    
    char ruta[MAX_CAMPO * 2];
    snprintf(ruta, sizeof(ruta), "data/%s.db", nombre_bd);
    
    FILE *fp = fopen(ruta, "a");
    if (fp) {
        fclose(fp);
        out("[OK] Base de datos '%s' creada\n", nombre_bd);
        return 1;
    }
    
    out("[ERROR] No se pudo crear la base de datos\n");
    return -1;
}

int parsear_usar(char *entrada) {
    char nombre_bd[MAX_CAMPO];
    int n = sscanf(entrada + 4, "%s", nombre_bd);
    
    if (n != 1) {
        out("[ERROR] Uso: usar <nombre_bd>\n");
        out("Ejemplo: usar mi_empresa\n");
        return -1;
    }
    
    char ruta[MAX_CAMPO * 2];
    snprintf(ruta, sizeof(ruta), "data/%s.db", nombre_bd);
    
    FILE *fp = fopen(ruta, "r");
    if (!fp) {
        out("[WARN] La base de datos '%s' no existe. Se creara al usarla.\n", nombre_bd);
    } else {
        fclose(fp);
    }
    
    cambiar_bd(nombre_bd);
    out("[OK] Usando base de datos: %s\n", nombre_bd);
    return 0;
}

int parsear_mostrarbd() {
    DIR *dir = opendir("data");
    struct dirent *entry;
    int count = 0;
    
    out("\n--- Bases de Datos ---\n");
    
    if (!dir) {
        out("(No hay bases de datos)\n");
        return 0;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        char *ext = strstr(entry->d_name, ".db");
        if (ext && strcmp(ext, ".db") == 0) {
            char nombre[MAX_CAMPO];
            strncpy(nombre, entry->d_name, strlen(entry->d_name) - 3);
            nombre[strlen(entry->d_name) - 3] = '\0';
            
            if (strcmp(nombre, bd_actual) == 0) {
                out("  * %s (actual)\n", nombre);
            } else {
                out("    %s\n", nombre);
            }
            count++;
        }
    }
    
    closedir(dir);
    
    if (count == 0) {
        out("(No hay bases de datos)\n");
    }
    
    return count;
}

int parsear_eliminarbd(char *entrada) {
    char nombre_bd[MAX_CAMPO];
    int n = sscanf(entrada + 10, "%s", nombre_bd);
    
    if (n != 1) {
        out("[ERROR] Uso: eliminarbd <nombre>\n");
        out("Ejemplo: eliminarbd empresa\n");
        return -1;
    }
    
    if (strcmp(nombre_bd, bd_actual) == 0) {
        out("[ERROR] No puedes eliminar la base de datos en uso.\n");
        out("Usa 'usar otra_base' primero.\n");
        return -1;
    }
    
    char ruta[MAX_CAMPO * 2];
    snprintf(ruta, sizeof(ruta), "data/%s.db", nombre_bd);
    
    if (remove(ruta) == 0) {
        out("[OK] Base de datos '%s' eliminada\n", nombre_bd);
        return 1;
    }
    
    out("[ERROR] No se pudo eliminar. Verifica el nombre.\n");
    return -1;
}

int parsear_infobd() {
    const char *ruta = obtener_ruta_bd();
    FILE *fp = fopen(ruta, "r");
    int registros = 0;
    int tablas = 0;
    char tabla_anterior[MAX_CAMPO] = "";
    char linea[MAX_LINEA];
    
    out("\n=== Info Base de Datos ===\n");
    out("Nombre: %s\n", bd_actual);
    
    if (!fp) {
        out("Registros: 0\n");
        out("Tablas: 0\n");
        return 0;
    }
    
    while (fgets(linea, MAX_LINEA, fp)) {
        char tabla[MAX_CAMPO];
        sscanf(linea, "%[^:]", tabla);
        
        if (strcmp(tabla, tabla_anterior) != 0) {
            tablas++;
            strcpy(tabla_anterior, tabla);
        }
        registros++;
    }
    
    fclose(fp);
    
    out("Registros: %d\n", registros);
    out("Tablas: %d\n", tablas);
    out("Archivo: %s\n", ruta);
    
    return 1;
}

int parsear_listar_tablas() {
    const char *ruta = obtener_ruta_bd();
    FILE *fp = fopen(ruta, "r");
    char tabla_anterior[MAX_CAMPO] = "";
    char linea[MAX_LINEA];
    int tablas = 0;
    
    out("\n--- Tablas en '%s' ---\n", bd_actual);
    
    if (!fp) {
        out("(Base de datos vacia)\n");
        return 0;
    }
    
    while (fgets(linea, MAX_LINEA, fp)) {
        char tabla[MAX_CAMPO];
        sscanf(linea, "%[^:]", tabla);
        
        if (strcmp(tabla, tabla_anterior) != 0) {
            out("  - %s\n", tabla);
            strcpy(tabla_anterior, tabla);
            tablas++;
        }
    }
    
    fclose(fp);
    
    if (tablas == 0) {
        out("(No hay tablas)\n");
    }
    
    return tablas;
}

int parsear_limpiar() {
    const char *ruta = obtener_ruta_bd();
    FILE *fp = fopen(ruta, "w");
    if (fp) {
        fclose(fp);
        out("[OK] Base de datos '%s' limpiada\n", bd_actual);
        return 1;
    }
    out("[ERROR] No se pudo limpiar la base de datos\n");
    return -1;
}
#include "gestor_arch.h"
#include "transaccion.h"
#include "parser.h"
#include "indice.h"
#include <string.h>
#include <ctype.h>

static FILE *archivo_db = NULL;

const char* obtener_ruta_bd() {
    static char ruta[256];
    snprintf(ruta, sizeof(ruta), "data/%s.db", obtener_bd_actual());
    return ruta;
}

int iniciar_archivo() {
    const char *ruta = obtener_ruta_bd();
    archivo_db = fopen(ruta, "a");
    if (archivo_db == NULL) {
        perror("Error al crear archivo");
        return -1;
    }
    fclose(archivo_db);
    out("[OK] Base de datos '%s' iniciada\n", obtener_bd_actual());
    return 0;
}

int es_entero(const char *valor) {
    if (valor == NULL || strlen(valor) == 0) return 0;
    for (int i = 0; valor[i]; i++) {
        if (!isdigit(valor[i]) && valor[i] != '-') return 0;
    }
    return 1;
}

int escribir_registro(const char *tabla, const char *clave, const char *valor) {
    const char *ruta = obtener_ruta_bd();
    FILE *fp = fopen(ruta, "a");
    if (fp == NULL) {
        perror("Error al abrir archivo");
        return -1;
    }
    
    if (es_entero(valor)) {
        fprintf(fp, "%s:%s:i:%s\n", tabla, clave, valor);
    } else {
        fprintf(fp, "%s:%s:t:%s\n", tabla, clave, valor);
    }
    fclose(fp);
    return 0;
}

int escribir_registro_entero(const char *tabla, const char *clave, int valor) {
    char val_str[MAX_VALOR];
    snprintf(val_str, sizeof(val_str), "%d", valor);
    return escribir_registro(tabla, clave, val_str);
}

int escribir_registro_texto(const char *tabla, const char *clave, const char *valor) {
    return escribir_registro(tabla, clave, valor);
}

int existe_tabla(const char *tabla) {
    const char *ruta = obtener_ruta_bd();
    char linea[MAX_LINEA];
    FILE *fp = fopen(ruta, "r");
    if (fp == NULL) {
        return 0;
    }
    while (fgets(linea, MAX_LINEA, fp)) {
        linea[strcspn(linea, "\n")] = 0;
        if (linea[0] == '#' || strlen(linea) == 0) continue;
        char tabla_arch[MAX_CAMPO];
        sscanf(linea, "%[^:]", tabla_arch);
        if (strcmp(tabla_arch, tabla) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int crear_tabla_en_bd(const char *tabla) {
    if (existe_tabla(tabla)) {
        return 1;
    }
    const char *ruta = obtener_ruta_bd();
    FILE *fp = fopen(ruta, "a");
    if (fp) {
        fprintf(fp, "#%s:new:0\n", tabla);
        fclose(fp);
        return 1;
    }
    return 0;
}

int leer_todos(const char *tabla) {
    const char *ruta = obtener_ruta_bd();
    char linea[MAX_LINEA];
    FILE *fp = fopen(ruta, "r");
    int encontrados = 0;
    
    if (fp == NULL) {
        out("[ERROR] No se pudo abrir la base de datos.\n");
        return -1;
    }
    
    out("\n--- Tabla: %s (%s) ---\n", tabla, obtener_bd_actual());
    out("%-20s | %-30s | %s\n", "CLAVE", "VALOR", "TIPO");
    out("---------------------------------------------------------\n");
    
    while (fgets(linea, MAX_LINEA, fp)) {
        linea[strcspn(linea, "\n")] = 0;
        
        if (linea[0] == '#') continue;
        
        char tabla_arch[MAX_CAMPO], clave_arch[MAX_CAMPO], tipo[2], valor[MAX_VALOR];
        if (sscanf(linea, "%[^:]:%[^:]:%[^:]:%[^\n]", tabla_arch, clave_arch, tipo, valor) >= 4) {
            if (strcmp(tabla_arch, tabla) == 0) {
                out("%-20s | %-30s | %s\n", clave_arch, valor, tipo[0] == 'i' ? "INT" : "TEXT");
                encontrados++;
            }
        }
    }
    
    fclose(fp);
    
    if (encontrados == 0) {
        out("(No hay registros)\n");
    }
    out("--- Total: %d registros ---\n", encontrados);
    
    return encontrados;
}

int buscar_registro(const char *tabla, const char *clave, Registro *reg) {
    const char *ruta = obtener_ruta_bd();
    char linea[MAX_LINEA];
    FILE *fp = fopen(ruta, "r");
    
    if (fp == NULL) {
        return -1;
    }
    
    TablaIndice *indice = indice_obtener_global();
    long offset = indice_buscar(indice, tabla, clave);
    
    if (offset >= 0) {
        fseek(fp, offset, SEEK_SET);
        if (fgets(linea, MAX_LINEA, fp)) {
            linea[strcspn(linea, "\n")] = 0;
            char tabla_arch[MAX_CAMPO], clave_arch[MAX_CAMPO], tipo[2], valor[MAX_VALOR];
            if (sscanf(linea, "%[^:]:%[^:]:%[^:]:%[^\n]", tabla_arch, clave_arch, tipo, valor) >= 4) {
                if (strcmp(tabla_arch, tabla) == 0 && strcmp(clave_arch, clave) == 0) {
                    strcpy(reg->nombre, tabla);
                    strcpy(reg->clave, clave);
                    reg->tipo = (tipo[0] == 'i') ? TIPO_ENTERO : TIPO_TEXTO;
                    if (reg->tipo == TIPO_ENTERO) {
                        reg->valor.entero = atoi(valor);
                    } else {
                        strcpy(reg->valor.texto, valor);
                    }
                    fclose(fp);
                    return 1;
                }
            }
        }
    }
    
    rewind(fp);
    while (fgets(linea, MAX_LINEA, fp)) {
        linea[strcspn(linea, "\n")] = 0;
        if (linea[0] == '#' || strlen(linea) == 0) continue;
        
        char tabla_arch[MAX_CAMPO], clave_arch[MAX_CAMPO], tipo[2], valor[MAX_VALOR];
        if (sscanf(linea, "%[^:]:%[^:]:%[^:]:%[^\n]", tabla_arch, clave_arch, tipo, valor) >= 4) {
            if (strcmp(tabla_arch, tabla) == 0 && strcmp(clave_arch, clave) == 0) {
                strcpy(reg->nombre, tabla);
                strcpy(reg->clave, clave);
                reg->tipo = (tipo[0] == 'i') ? TIPO_ENTERO : TIPO_TEXTO;
                if (reg->tipo == TIPO_ENTERO) {
                    reg->valor.entero = atoi(valor);
                } else {
                    strcpy(reg->valor.texto, valor);
                }
                fclose(fp);
                return 1;
            }
        }
    }
    
    fclose(fp);
    return 0;
}

int modificar_registro(const char *tabla, const char *clave, const char *nuevo_valor) {
    const char *ruta = obtener_ruta_bd();
    char linea[MAX_LINEA];
    char temp[MAX_LINEA * 100];
    int encontrado = 0;
    
    FILE *fp = fopen(ruta, "r");
    if (fp == NULL) {
        return -1;
    }
    
    temp[0] = '\0';
    
    while (fgets(linea, MAX_LINEA, fp)) {
        linea[strcspn(linea, "\n")] = 0;
        if (linea[0] == '#' || strlen(linea) == 0) {
            strcat(temp, linea);
            strcat(temp, "\n");
            continue;
        }
        
        char tabla_arch[MAX_CAMPO], clave_arch[MAX_CAMPO];
        sscanf(linea, "%[^:]:%[^:]", tabla_arch, clave_arch);
        
        if (strcmp(tabla_arch, tabla) == 0 && strcmp(clave_arch, clave) == 0) {
            if (es_entero(nuevo_valor)) {
                sprintf(linea, "%s:%s:i:%s\n", tabla, clave, nuevo_valor);
            } else {
                sprintf(linea, "%s:%s:t:%s\n", tabla, clave, nuevo_valor);
            }
            encontrado = 1;
        }
        strcat(temp, linea);
        strcat(temp, "\n");
    }
    fclose(fp);
    
    if (!encontrado) {
        out("[ERROR] Registro no encontrado: %s:%s\n", tabla, clave);
        return 0;
    }
    
    fp = fopen(ruta, "w");
    fputs(temp, fp);
    fclose(fp);
    
    return 1;
}

int borrar_registro(const char *tabla, const char *clave) {
    const char *ruta = obtener_ruta_bd();
    char linea[MAX_LINEA];
    char temp[MAX_LINEA * 100];
    int encontrado = 0;
    
    FILE *fp = fopen(ruta, "r");
    if (fp == NULL) {
        return -1;
    }
    
    temp[0] = '\0';
    
    while (fgets(linea, MAX_LINEA, fp)) {
        linea[strcspn(linea, "\n")] = 0;
        if (linea[0] == '#' || strlen(linea) == 0) {
            strcat(temp, linea);
            strcat(temp, "\n");
            continue;
        }
        
        char tabla_arch[MAX_CAMPO], clave_arch[MAX_CAMPO];
        sscanf(linea, "%[^:]:%[^:]", tabla_arch, clave_arch);
        
        if (strcmp(tabla_arch, tabla) == 0 && strcmp(clave_arch, clave) == 0) {
            encontrado = 1;
            continue;
        }
        strcat(temp, linea);
        strcat(temp, "\n");
    }
    fclose(fp);
    
    if (!encontrado) {
        out("[ERROR] Registro no encontrado: %s:%s\n", tabla, clave);
        return 0;
    }
    
    fp = fopen(ruta, "w");
    fputs(temp, fp);
    fclose(fp);
    
    return 1;
}

int modificar_registro_entero(const char *tabla, const char *clave, int nuevo_valor) {
    char val_str[MAX_VALOR];
    snprintf(val_str, sizeof(val_str), "%d", nuevo_valor);
    return modificar_registro(tabla, clave, val_str);
}

int modificar_registro_texto(const char *tabla, const char *clave, const char *nuevo_valor) {
    return modificar_registro(tabla, clave, nuevo_valor);
}